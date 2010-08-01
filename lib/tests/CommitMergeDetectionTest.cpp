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



class CommitMergeDetectionTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			commit = 0;

			cloneFrom("CommitMergeDetectionTestRepo");
		}

		void init(){
			GitTestBase::init();

			commit = new Git::Commit("b462958a492e9abaaa3bd2725639932b5fd551d9", *repo);
			commit->d->message = "alibi"; // to inhibit automatic popultation
		}

		void cleanup() {
			delete commit;
			GitTestBase::cleanup();
		}

		void testMergeDetectionWithNoParents();
		void testMergeDetectionWithOneParent();
		void testMergeDetectionWithMoreParents();

	private:
		Git::Commit *commit;
};

QTEST_KDEMAIN_CORE(CommitMergeDetectionTest)



void CommitMergeDetectionTest::testMergeDetectionWithNoParents()
{
	QVERIFY(commit->d->parents.isEmpty());

	QVERIFY(!commit->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionWithOneParent()
{
	commit->d->parents << new Git::Commit("1234567");
	QCOMPARE(commit->d->parents.size(), 1);

	QVERIFY(!commit->isMerge());
}

void CommitMergeDetectionTest::testMergeDetectionWithMoreParents()
{
	commit->d->parents << new Git::Commit("1234567");
	commit->d->parents << new Git::Commit("2345678");
	QCOMPARE(commit->d->parents.size(), 2);
	QVERIFY(commit->isMerge());

	commit->d->parents << new Git::Commit("3456789");
	QCOMPARE(commit->d->parents.size(), 3);
	QVERIFY(commit->isMerge());

	commit->d->parents << new Git::Commit("4567890");
	QCOMPARE(commit->d->parents.size(), 4);
	QVERIFY(commit->isMerge());
}

#include "CommitMergeDetectionTest.moc"
