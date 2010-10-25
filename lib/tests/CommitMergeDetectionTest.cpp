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
#include "Git/Commit_p.h"



class CommitMergeDetectionTest : public GitTestBase
{
	Q_OBJECT

	Git::Commit *commit;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			commit = 0;

			cloneFrom("CommitMergeDetectionTestRepo");
		}

		void init(){
			GitTestBase::init();

			Git::Id id("b462958a492e9abaaa3bd2725639932b5fd551d9", *repo);
			commit = new Git::Commit(id, *repo);
			commit->d->message = "alibi"; // to inhibit automatic popultation
		}

		void cleanup() {
			delete commit;
			GitTestBase::cleanup();
		}

		void testMergeDetectionWithNoParents()
		{
			QVERIFY(commit->d->parentIds.isEmpty());

			QVERIFY(!commit->isMerge());
		}

		void testMergeDetectionWithOneParent()
		{
			commit->d->parentIds << Git::Id("abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d", *repo);
			QCOMPARE(commit->d->parentIds.size(), 1);

			QVERIFY(!commit->isMerge());
		}

		void testMergeDetectionWithMoreParents()
		{
			commit->d->parentIds << Git::Id("abffc0ae9ba476fe1e9a30fa2c8903113dbadb3d", *repo);
			commit->d->parentIds << Git::Id("6421f09a627d8ea6a85a9155e481cae7ed483b50", *repo);
			QCOMPARE(commit->d->parentIds.size(), 2);
			QVERIFY(commit->isMerge());

			commit->d->parentIds << Git::Id("4262f0d5b0d062a0d655f16c2fc372c92689c853", *repo);
			QCOMPARE(commit->d->parentIds.size(), 3);
			QVERIFY(commit->isMerge());
		}
};

QTEST_KDEMAIN_CORE(CommitMergeDetectionTest);

#include "CommitMergeDetectionTest.moc"
