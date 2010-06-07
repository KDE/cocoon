/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009  Riyad Preukschas <riyad@informatik.uni-bremen.de>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "GitTestBase.h"

#include "Git/Repo_p.h"



class RepoCommitsCachingTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RepoCommitsCachingTestRepo");
		}



		void testCommitsCachingSame() {
			QVERIFY(repo->d->commits.isEmpty());

			Git::CommitList commits1 = repo->commits();
			QCOMPARE(commits1.size(), 4);
			QCOMPARE(repo->d->commits.size(), 1);
			QVERIFY(repo->d->commits.contains(QString()));

			Git::CommitList commits2 = repo->commits();
			QCOMPARE(commits2.size(), 4);
			QCOMPARE(repo->d->commits.size(), 1);
			QVERIFY(repo->d->commits.contains(QString()));

			QCOMPARE(commits2, commits1);
		}

		void testCommitsCachingDifferent() {
			QVERIFY(repo->d->commits.isEmpty());

			Git::CommitList commits1 = repo->commits("master");
			QCOMPARE(commits1.size(), 4);
			QCOMPARE(repo->d->commits.size(), 1);
			QVERIFY(repo->d->commits.contains("master"));

			Git::CommitList commits2 = repo->commits("branch");
			QCOMPARE(commits2.size(), 2);
			QCOMPARE(repo->d->commits.size(), 2);
			QVERIFY(repo->d->commits.contains("branch"));
		}

		void testCommitsReset() {
			QVERIFY(repo->d->commits.isEmpty());

			repo->commits();
			QVERIFY(!repo->d->commits.isEmpty());

			repo->resetCommits();
			QVERIFY(repo->d->commits.isEmpty());
		}

		void testReset() {
			QVERIFY(repo->d->commits.isEmpty());

			repo->commits();
			QVERIFY(!repo->d->commits.isEmpty());

			repo->reset();
			QVERIFY(repo->d->commits.isEmpty());
		}
};

QTEST_KDEMAIN_CORE(RepoCommitsCachingTest);

#include "RepoCommitsCachingTest.moc"
