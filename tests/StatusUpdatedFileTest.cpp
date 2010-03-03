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



class StatusUpdatedFileTest : public GitTestBase
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

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusUpdatedFileTest)



void StatusUpdatedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	QProcess::execute("mkdir", QStringList() << "-p" << workingDir);
	QProcess::execute("git", gitBasicOpts() << "init");

	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	writeToFile("updated.txt", "foo\nbar");

	QProcess::execute("git", gitBasicOpts() << "add" << "updated.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Committed file to be modified.");

	writeToFile("updated.txt", "foo\nbar\nbaz\n");

	QProcess::execute("git", gitBasicOpts() << "add" << "updated.txt");
}



void StatusUpdatedFileTest::testUpdatedFile_lsFiles()
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

void StatusUpdatedFileTest::testUpdatedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void StatusUpdatedFileTest::testUpdatedFile_diffIndex()
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

void StatusUpdatedFileTest::testUpdatedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusUpdatedFileTest::testUpdatedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusUpdatedFileTest::testUpdatedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("updated.txt").isEmpty());
}

void StatusUpdatedFileTest::testUpdatedFileIsModified()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("updated.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isModified());
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void StatusUpdatedFileTest::testUpdatedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void StatusUpdatedFileTest::testUpdatedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob() == file->blob("index"));
}

void StatusUpdatedFileTest::testUpdatedFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
}

void StatusUpdatedFileTest::testUpdatedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("index") == "foo\nbar\nbaz\n");
}

void StatusUpdatedFileTest::testUpdatedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("updated.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar");
}

void StatusUpdatedFileTest::testUpdatedFileDiffIsCorrect()
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
	QVERIFY(diff == "--- a/updated.txt\n+++ b/updated.txt\n@@ -1,2 +1,3 @@\n foo\n-bar\n\\ No newline at end of file\n+bar\n+baz\n");
}

#include "StatusUpdatedFileTest.moc"
