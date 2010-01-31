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



class RepoCommitsCachingTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");
			QProcess::execute("git", gitBasicOpts() << "checkout" << "-b" << "branch");
			QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty branch commit.");
			QProcess::execute("git", gitBasicOpts() << "checkout" << "master");
			QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty master commit.");
			QProcess::execute("git", gitBasicOpts() << "merge" << "branch");
		}



		void testCommitsCachingSame() {
			QVERIFY(repo->m_commits.isEmpty());

			Git::CommitList commits1 = repo->commits();
			QCOMPARE(commits1.size(), 4);
			QCOMPARE(repo->m_commits.size(), 1);
			QVERIFY(repo->m_commits.contains(QString()));

			Git::CommitList commits2 = repo->commits();
			QCOMPARE(commits2.size(), 4);
			QCOMPARE(repo->m_commits.size(), 1);
			QVERIFY(repo->m_commits.contains(QString()));

			QCOMPARE(commits2, commits1);
		}

		void testCommitsCachingDifferent() {
			QVERIFY(repo->m_commits.isEmpty());

			Git::CommitList commits1 = repo->commits("master");
			QCOMPARE(commits1.size(), 4);
			QCOMPARE(repo->m_commits.size(), 1);
			QVERIFY(repo->m_commits.contains("master"));

			Git::CommitList commits2 = repo->commits("branch");
			QCOMPARE(commits2.size(), 2);
			QCOMPARE(repo->m_commits.size(), 2);
			QVERIFY(repo->m_commits.contains("branch"));
		}

		void testCommitsReset() {
			QVERIFY(repo->m_commits.isEmpty());

			repo->commits();
			QVERIFY(!repo->m_commits.isEmpty());

			repo->resetCommits();
			QVERIFY(repo->m_commits.isEmpty());
		}

		void testReset() {
			QVERIFY(repo->m_commits.isEmpty());

			repo->commits();
			QVERIFY(!repo->m_commits.isEmpty());

			repo->reset();
			QVERIFY(repo->m_commits.isEmpty());
		}
};

QTEST_KDEMAIN_CORE(RepoCommitsCachingTest);

#include "RepoCommitsCachingTest.moc"
