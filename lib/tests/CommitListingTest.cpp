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

#include "Git/Commit.h"
#include "Git/Head.h"
#include "Git/LooseStorage.h"



class CommitListingTest : public GitTestBase
{
	Q_OBJECT

	Git::LooseStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("CommitListingTestRepo");
		}

		void init() {
			GitTestBase::init();
			storage = new Git::LooseStorage(*repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}



		void shouldHaveCorrectNumberOfCommitsOnMaster() {
			Git::CommitList commits = Git::Commit::allReachableFrom(Git::Head("master", *repo));
			QCOMPARE(commits.size(), 4);
		}

		void shouldHaveCorrectNumberOfCommitsOnBranch() {
			Git::CommitList commits = Git::Commit::allReachableFrom(Git::Head("branch", *repo));
			QCOMPARE(commits.size(), 2);
		}

		void firstCommitInListShouldBeTheRefCommit() {
			Git::Commit* headCommit = repo->head()->commit();
			Git::Commit* listCommit = Git::Commit::allReachableFrom(*repo->head())[0];

			foreach (Git::Commit *commit, Git::Commit::allReachableFrom(*repo->head())) {
				qDebug() << commit->committedAt().toString() << commit->id() << commit->summary();
			}

			QCOMPARE(listCommit->id(), headCommit->id());
			QCOMPARE(listCommit, headCommit);
		}
};

QTEST_KDEMAIN_CORE(CommitListingTest);

#include "CommitListingTest.moc"
