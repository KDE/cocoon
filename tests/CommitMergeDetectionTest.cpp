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

#include "Commit.h"
#include "LooseStorage.h"



class CommitMergeDetectionTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void init(){
			GitTestBase::init();
			QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");
			storage = new Git::LooseStorage(*repo);

			QString id = repo->commits()[0]->id();
			commit = new Git::Commit(id, *storage);
		}

		void cleanup() {
			delete commit;
			delete storage;
			GitTestBase::cleanup();
		}

		void testMergeDetectionWithNoParents();
		void testMergeDetectionWithOneParent();
		void testMergeDetectionWithMoreParents();

	private:
		Git::LooseStorage *storage;
		Git::Commit *commit;
};

QTEST_KDEMAIN_CORE(CommitMergeDetectionTest)



void CommitMergeDetectionTest::testMergeDetectionWithNoParents()
{
	QVERIFY(commit->m_parents.isEmpty());

	QVERIFY(!commit->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionWithOneParent()
{
	commit->m_parents << new Git::Commit("1234567", *storage);
	QVERIFY(commit->m_parents.size() == 1);

	QVERIFY(!commit->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionWithMoreParents()
{
	commit->m_parents << new Git::Commit("1234567", *storage);
	commit->m_parents << new Git::Commit("2345678", *storage);
	QVERIFY(commit->m_parents.size() == 2);
	QVERIFY(commit->isMerge());

	commit->m_parents << new Git::Commit("3456789", *storage);
	QVERIFY(commit->m_parents.size() == 3);
	QVERIFY(commit->isMerge());

	commit->m_parents << new Git::Commit("4567890", *storage);
	QVERIFY(commit->m_parents.size() == 4);
	QVERIFY(commit->isMerge());
}

#include "CommitMergeDetectionTest.moc"
