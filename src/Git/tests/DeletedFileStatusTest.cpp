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



class DeletedFileStatusTest : public GitTestBase
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

		void testDeletedFile_lsFiles();
		void testDeletedFile_diffFiles();
		void testDeletedFile_diffIndex();
		void testDeletedFile_diffUntrackedFiles();
		void testDeletedFile_diffIgnoredFiles();

		void testDeletedFileHasStatus();
		void testDeletedFileIsDeleted();
		void testDeletedFileIsUnstaged();
		void testDeletedFileDefaultBlobIsFileBlob();
		void testDeletedFileFileBlobIsEmpty();
		void testDeletedFileIndexBlobIsCorrect();
		void testDeletedFileRepoBlobIsCorrect();
		void testDeletedFileDiffIsCorrect();

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(DeletedFileStatusTest)



void DeletedFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	writeToFile("deleted.txt", "foo\nbar\nbaz\n");

	QProcess::execute("git", gitBasicOpts() << "add" << "deleted.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added file to be deleted.");

	deleteFile("deleted.txt");
}



void DeletedFileStatusTest::testDeletedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QVERIFY(file->idIndex() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "deleted.txt");
	QVERIFY(file->status() == Git::StatusFile::None);
}

void DeletedFileStatusTest::testDeletedFile_diffFiles()
{
	QVERIFY(status->diffFiles().size() == 1);

	Git::StatusFile *file = status->diffFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "deleted.txt");
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void DeletedFileStatusTest::testDeletedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "deleted.txt");
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void DeletedFileStatusTest::testDeletedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void DeletedFileStatusTest::testDeletedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void DeletedFileStatusTest::testDeletedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("deleted.txt").isEmpty());
}

void DeletedFileStatusTest::testDeletedFileIsDeleted()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("deleted.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isDeleted());
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void DeletedFileStatusTest::testDeletedFileIsUnstaged()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(!file->isStaged());

	QVERIFY(!file->changesStaged());
	QVERIFY(file->changesUnstaged());
}

void DeletedFileStatusTest::testDeletedFileDefaultBlobIsFileBlob()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->blob() == file->blob("file"));
}

void DeletedFileStatusTest::testDeletedFileFileBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->blob("file").isNull());
}

void DeletedFileStatusTest::testDeletedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar\nbaz\n");
}

void DeletedFileStatusTest::testDeletedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar\nbaz\n");
}

void DeletedFileStatusTest::testDeletedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('deleted.txt', d.a_path)
		assert_equal('deleted.txt', d.b_path)
		assert(!d.new_file)
		assert(d.deleted_file)
		assert_equal('86e041d', d.a_sha)
		assert_nil(d.b_sha)
		assert_equal("--- a/deleted.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- a/deleted.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n");
}

#include "DeletedFileStatusTest.moc"
