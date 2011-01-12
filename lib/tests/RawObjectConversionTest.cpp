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

#include "Git/Blob.h"
#include "Git/Commit.h"
#include "Git/Tree.h"



class RawObjectConversionTest : public GitTestBase
{
	Q_OBJECT

	Git::Id blobId;
	Git::Id commitId;
	Git::Id treeId;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RawObjectConversionTestRepo");
		}

		void init() {
			GitTestBase::init();

			blobId = repo->idFor("86e041d");
			commitId = repo->idFor("c56dada");
			treeId = repo->idFor("5b36b1f");
		}



		void conversionToBlobShouldBeSameObjectIfBlob() {
			Git::RawObject *obj = &repo->object(blobId);

			Git::Blob *blob = &obj->toBlob();

			QVERIFY(blob == obj);
		}

		void conversionToBlobShouldBeInvalidBlobIfNotBlob() {
			Git::RawObject *obj = &repo->object(commitId);

			Git::Blob *blob = &obj->toBlob();

			QVERIFY(blob != obj);
			QVERIFY(blob == &Git::Blob::invalid());
		}

		void conversionToCommitShouldBeSameObjectIfCommit() {
			Git::RawObject *obj = &repo->object(commitId);

			Git::Commit *commit = &obj->toCommit();

			QVERIFY(commit == obj);
		}

		void conversionToCommitShouldBeInvalidCommitIfNotCommit() {
			Git::RawObject *obj = &repo->object(blobId);

			Git::Commit *commit = &obj->toCommit();

			QVERIFY(commit != obj);
			QVERIFY(commit == &Git::Commit::invalid());
		}

		void conversionToTreeShouldBeSameObjectIfTree() {
			Git::RawObject *obj = &repo->object(treeId);

			Git::Tree *tree = &obj->toTree();

			QVERIFY(tree == obj);
		}

		void conversionToTreeShouldBeInvalidTreeIfNotTree() {
			Git::RawObject *obj = &repo->object(blobId);

			Git::Tree *tree = &obj->toTree();

			QVERIFY(tree != obj);
			QVERIFY(tree == &Git::Tree::invalid());
		}
};

QTEST_KDEMAIN_CORE(RawObjectConversionTest);

#include "RawObjectConversionTest.moc"
