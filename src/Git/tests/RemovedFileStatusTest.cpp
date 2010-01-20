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



class RemovedFileStatusTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();

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
};

QTEST_KDEMAIN_CORE(RemovedFileStatusTest)



void RemovedFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();

	workingDir = QDir::temp().filePath("removed_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	qDebug() << workingDir;

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("removed.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "removed.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added file to be deleted.");

	QProcess::execute("git", gitBasicOpts() << "rm" << "removed.txt"); // also stages the changes
}



void RemovedFileStatusTest::testRemovedFile_lsFiles()
{
	QVERIFY(status->lsFiles().isEmpty());
}

void RemovedFileStatusTest::testRemovedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void RemovedFileStatusTest::testRemovedFile_diffIndex()
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

void RemovedFileStatusTest::testRemovedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void RemovedFileStatusTest::testRemovedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void RemovedFileStatusTest::testRemovedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("removed.txt").isEmpty());
}

void RemovedFileStatusTest::testRemovedFileIsDeleted()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("removed.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isDeleted());
	QVERIFY(file->status() == Git::StatusFile::Deleted);
}

void RemovedFileStatusTest::testRemovedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void RemovedFileStatusTest::testRemovedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob() == file->blob("index"));
}

void RemovedFileStatusTest::testRemovedFileFileBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("file").isNull());
}

void RemovedFileStatusTest::testRemovedFileIndexBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("index").isNull());
}

void RemovedFileStatusTest::testRemovedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("removed.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar\nbaz\n");
}

void RemovedFileStatusTest::testRemovedFileDiffIsCorrect()
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
	QVERIFY("--- a/removed.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n");
}

#include "RemovedFileStatusTest.moc"
