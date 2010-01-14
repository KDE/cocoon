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



class NewlyAddedFileStatusTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();

		void testNewlyAddedFile_lsFiles();
		void testNewlyAddedFile_diffFiles();
		void testNewlyAddedFile_diffIndex();
		void testNewlyAddedFile_diffUntrackedFiles();
		void testNewlyAddedFile_diffIgnoredFiles();

		void testNewlyAddedFileHasStatus();
		void testNewlyAddedFileIsAdded();
		void testNewlyAddedFileIsStaged();
		void testNewlyAddedFileDefaultBlobIsIndexBlob();
		void testNewlyAddedFileFileBlobIsCorrect();
		void testNewlyAddedFileIndexBlobIsCorrect();
		void testNewlyAddedFileRepoBlobIsEmpty();
		void testNewlyAddedFileDiffIsCorrect();
};

QTEST_KDEMAIN_CORE(NewlyAddedFileStatusTest)



void NewlyAddedFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();

	workingDir = QDir::temp().filePath("newly_added_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	qDebug() << workingDir;

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("newly_added.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "newly_added.txt");
}



void NewlyAddedFileStatusTest::testNewlyAddedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QVERIFY(file->idIndex() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "newly_added.txt");
	QVERIFY(file->status().isNull());
}

void NewlyAddedFileStatusTest::testNewlyAddedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void NewlyAddedFileStatusTest::testNewlyAddedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "newly_added.txt");
	QVERIFY(file->status() == "A");
}

void NewlyAddedFileStatusTest::testNewlyAddedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void NewlyAddedFileStatusTest::testNewlyAddedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void NewlyAddedFileStatusTest::testNewlyAddedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("newly_added.txt").isEmpty());
}

void NewlyAddedFileStatusTest::testNewlyAddedFileIsAdded()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("newly_added.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isAdded());
	QVERIFY(file->status() == "A");
}

void NewlyAddedFileStatusTest::testNewlyAddedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void NewlyAddedFileStatusTest::testNewlyAddedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->blob() == file->blob("index"));
}

void NewlyAddedFileStatusTest::testNewlyAddedFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
}

void NewlyAddedFileStatusTest::testNewlyAddedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->blob("index") == "foo\nbar\nbaz\n");
}

void NewlyAddedFileStatusTest::testNewlyAddedFileRepoBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->blob("repo").isNull());
}

void NewlyAddedFileStatusTest::testNewlyAddedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('newly_added.txt', d.a_path)
		assert_equal('newly_added.txt', d.b_path)
		assert(d.new_file)
		assert(!d.deleted_file)
		assert_nil(d.a_sha)
		assert_equal('86e041d', d.b_sha)
		assert_equal("--- /dev/null\n+++ b/newly_added.txt\n@@ -0,0 +1,3 @@\n+foo\n+bar\n+baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- /dev/null\n+++ b/newly_added.txt\n@@ -0,0 +1,3 @@\n+foo\n+bar\n+baz\n");
}

#include "NewlyAddedFileStatusTest.moc"
