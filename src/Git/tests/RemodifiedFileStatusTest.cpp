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



class RemodifiedFileStatusTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();

		void testRemodifiedFile_lsFiles();
		void testRemodifiedFile_diffFiles();
		void testRemodifiedFile_diffIndex();
		void testRemodifiedFile_diffUntrackedFiles();
		void testRemodifiedFile_diffIgnoredFiles();

		void testRemodifiedFileHasStatus();
		void testRemodifiedFileIsModified();
		void testRemodifiedFileIsStagedAndUnstaged();
		void testUnstagedRemodifiedFileBlobsAreCorrect();
		void testStagedRemodifiedFileBlobsAreCorrect();
		void testUnstagedRemodifiedFileDiffIsCorrect();
		void testStagedRemodifiedFileDiffIsCorrect();
};

QTEST_KDEMAIN_CORE(RemodifiedFileStatusTest)



void RemodifiedFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();

	workingDir = QDir::temp().filePath("remodified_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	qDebug() << workingDir;

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("remodified.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "remodified.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Committed file to be modified.");

	file.open(QFile::ReadWrite);
	file.write("foo\nbar\n");
	file.close();

	QProcess::execute("git", gitBasicOpts() << "add" << "remodified.txt");

	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();
}



void RemodifiedFileStatusTest::testRemodifiedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QVERIFY(file->idIndex() == "3bd1f0e29744a1f32b08d5650e62e2e62afb177c");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "remodified.txt");
	QVERIFY(file->status().isNull());
}

void RemodifiedFileStatusTest::testRemodifiedFile_diffFiles()
{
	QVERIFY(status->diffFiles().size() == 1);

	Git::StatusFile *file = status->diffFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "3bd1f0e29744a1f32b08d5650e62e2e62afb177c");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "remodified.txt");
	QVERIFY(file->status() == "M");
}

void RemodifiedFileStatusTest::testRemodifiedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "19102815663d23f8b75a47e7a01965dcdc96468c");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "remodified.txt");
	QVERIFY(file->status() == "M");
}

void RemodifiedFileStatusTest::testRemodifiedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void RemodifiedFileStatusTest::testRemodifiedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void RemodifiedFileStatusTest::testRemodifiedFileHasStatus()
{
	QVERIFY(status->files().size() == 2);
	QVERIFY(!status->forFile("remodified.txt").isEmpty());
}

void RemodifiedFileStatusTest::testRemodifiedFileIsModified()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("remodified.txt");
	QVERIFY(fileStatus.size() == 2);

	Git::StatusFile *file = 0;

	file = fileStatus[0];
	QVERIFY(file->isModified());
	QVERIFY(file->status() == "M");

	file = fileStatus[1];
	QVERIFY(file->isModified());
	QVERIFY(file->status() == "M");
}

void RemodifiedFileStatusTest::testRemodifiedFileIsStagedAndUnstaged()
{
	Git::StatusFile *file = 0;

	file = status->forFile("remodified.txt")[0];
	QVERIFY(file->changesUnstaged());

	file = status->forFile("remodified.txt")[1];
	QVERIFY(file->changesStaged());
}

void RemodifiedFileStatusTest::testUnstagedRemodifiedFileBlobsAreCorrect()
{
	Git::StatusFile *file = status->unstagedFiles()[0];

	QVERIFY(file->blob() == file->blob("file"));
	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
	QVERIFY(file->blob("index") == "foo\nbar\n");
	QVERIFY(file->blob("repo") == "foo\nbar\n");
}

void RemodifiedFileStatusTest::testStagedRemodifiedFileBlobsAreCorrect()
{
	Git::StatusFile *file = status->stagedFiles()[0];

	QVERIFY(file->blob() == file->blob("index"));
	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
	QVERIFY(file->blob("index").isNull());
	QVERIFY(file->blob("repo") == "foo");
}

void RemodifiedFileStatusTest::testUnstagedRemodifiedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->unstagedFiles()[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('remodified.txt', d.a_path)
		assert_equal('remodified.txt', d.b_path)
		assert(!d.new_file)
		assert(!d.deleted_file)
		assert_equal('3bd1f0e', d.a_sha)
		assert_equal('86e041d', d.b_sha)
		assert_equal("--- a/remodified.txt\n+++ b/remodified.txt\n@@ -1,2 +1,3 @@\n foo\n bar\n+baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- a/remodified.txt\n+++ b/remodified.txt\n@@ -1,2 +1,3 @@\n foo\n bar\n+baz\n");
}

void RemodifiedFileStatusTest::testStagedRemodifiedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->stagedFiles()[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('remodified.txt', d.a_path)
		assert_equal('remodified.txt', d.b_path)
		assert(!d.new_file)
		assert(!d.deleted_file)
		assert_equal('1910281', d.a_sha)
		assert_equal('3bd1f0e', d.b_sha)
		assert_equal("--- a/remodified.txt\n+++ b/remodified.txt\n@@ -1 +1,2 @@\n-foo\n\\ No newline at end of file\n+foo\n+bar\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- a/remodified.txt\n+++ b/remodified.txt\n@@ -1 +1,2 @@\n-foo\n\\ No newline at end of file\n+foo\n+bar\n");
}

#include "RemodifiedFileStatusTest.moc"
