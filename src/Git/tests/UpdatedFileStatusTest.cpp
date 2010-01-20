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



class UpdatedFileStatusTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();

		void testUpdatedFile_lsFiles();
		void testUpdatedFile_diffFiles();
		void testUpdatedFile_diffIndex();
		void testUpdatedFile_diffUntrackedFiles();
		void testUpdatedFile_diffIgnoredFiles();

		void testUpdatedFileHasStatus();
		void testUpdatedFileIsModified();
		void testUpdatedFileIsStaged();
		void testUpdatedFileDefaultBlobIsIndexBlob();
		void testUpdatedFileFileBlobIsCorrect();
		void testUpdatedFileIndexBlobIsCorrect();
		void testUpdatedFileRepoBlobIsCorrect();
		void testUpdatedFileDiffIsCorrect();
};

QTEST_KDEMAIN_CORE(UpdatedFileStatusTest)



void UpdatedFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();

	workingDir = QDir::temp().filePath("updated_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	qDebug() << workingDir;

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("updated.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo\nbar");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "updated.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Committed file to be modified.");

	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "updated.txt");
}



void UpdatedFileStatusTest::testUpdatedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QVERIFY(file->idIndex() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "updated.txt");
	QVERIFY(file->status() == Git::StatusFile::None);
}

void UpdatedFileStatusTest::testUpdatedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void UpdatedFileStatusTest::testUpdatedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex() == "86e041dad66a19b9518b83b78865015f62662f75");
	QVERIFY(file->idRepo() == "a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "updated.txt");
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void UpdatedFileStatusTest::testUpdatedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void UpdatedFileStatusTest::testUpdatedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void UpdatedFileStatusTest::testUpdatedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("updated.txt").isEmpty());
}

void UpdatedFileStatusTest::testUpdatedFileIsModified()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("updated.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isModified());
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void UpdatedFileStatusTest::testUpdatedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void UpdatedFileStatusTest::testUpdatedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob() == file->blob("index"));
}

void UpdatedFileStatusTest::testUpdatedFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
}

void UpdatedFileStatusTest::testUpdatedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("index") == "foo\nbar\nbaz\n");
}

void UpdatedFileStatusTest::testUpdatedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar");
}

void UpdatedFileStatusTest::testUpdatedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('updated.txt', d.a_path)
		assert_equal('updated.txt', d.b_path)
		assert(!d.new_file)
		assert(!d.deleted_file)
		assert_equal('a907ec3', d.a_sha)
		assert_equal('86e041d', d.b_sha)
		assert_equal("--- a/updated.txt\n+++ b/updated.txt\n@@ -1,2 +1,3 @@\n foo\n-bar\n\\ No newline at end of file\n+bar\n+baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- a/updated.txt\n+++ b/updated.txt\n@@ -1,2 +1,3 @@\n foo\n-bar\n\\ No newline at end of file\n+bar\n+baz\n");
}

#include "UpdatedFileStatusTest.moc"
