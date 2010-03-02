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



class StatusRemovedFileTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void init() {
			GitTestBase::init();
			status = repo->status();
		}

		void cleanup() {
			repo->resetStatus();
			status = 0;
			GitTestBase::cleanup();
		}

		void testRemovedFile_lsFiles();
		void testRemovedFile_diffFiles();
		void testRemovedFile_diffIndex();
		void testRemovedFile_diffUntrackedFiles();
		void testRemovedFile_diffIgnoredFiles();

		void testRemovedFileHasStatus();
		void testRemovedFileIsDeleted();
		void testRemovedFileIsStaged();
		void testRemovedFileDefaultBlobIsIndexBlob();
		void testRemovedFileFileBlobIsEmpty();
		void testRemovedFileIndexBlobIsEmpty();
		void testRemovedFileRepoBlobIsCorrect();
		void testRemovedFileDiffIsCorrect();

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusRemovedFileTest)



void StatusRemovedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	cloneFrom("StatusRemovedFileTestRepo");
}



void StatusRemovedFileTest::testRemovedFile_lsFiles()
{
	QVERIFY(status->lsFiles().isEmpty());
}

void StatusRemovedFileTest::testRemovedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void StatusRemovedFileTest::testRemovedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "removed.txt");
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void StatusRemovedFileTest::testRemovedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusRemovedFileTest::testRemovedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusRemovedFileTest::testRemovedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("removed.txt").isEmpty());
}

void StatusRemovedFileTest::testRemovedFileIsDeleted()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("removed.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isDeleted());
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void StatusRemovedFileTest::testRemovedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void StatusRemovedFileTest::testRemovedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob() == file->blob("index"));
}

void StatusRemovedFileTest::testRemovedFileFileBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("file").isNull());
}

void StatusRemovedFileTest::testRemovedFileIndexBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("index").isNull());
}

void StatusRemovedFileTest::testRemovedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar\nbaz\n");
}

void StatusRemovedFileTest::testRemovedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('removed.txt', d.a_path)
		assert_equal('removed.txt', d.b_path)
		assert(!d.new_file)
		assert(d.deleted_file)
		assert_equal('86e041d', d.a_sha)
		assert_nil(d.b_sha)
		assert_equal("--- a/removed.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY(diff == "--- a/removed.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n");
}

#include "StatusRemovedFileTest.moc"
