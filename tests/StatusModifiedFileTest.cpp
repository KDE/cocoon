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



class StatusModifiedFileTest : public GitTestBase
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

		void testModifiedFile_lsFiles();
		void testModifiedFile_diffFiles();
		void testModifiedFile_diffIndex();
		void testModifiedFile_diffUntrackedFiles();
		void testModifiedFile_diffIgnoredFiles();

		void testModifiedFileHasStatus();
		void testModifiedFileIsModified();
		void testModifiedFileIsUnstaged();
		void testModifiedFileDefaultBlobIsFileBlob();
		void testModifiedFileFileBlobIsCorrect();
		void testModifiedFileIndexBlobIsCorrect();
		void testModifiedFileRepoBlobIsCorrect();
		void testModifiedFileDiffIsCorrect();

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusModifiedFileTest)



void StatusModifiedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	writeToFile("modified.txt", "foo\nbar");

	QProcess::execute("git", gitBasicOpts() << "add" << "modified.txt");
	QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Committed file to be modified.");

	writeToFile("modified.txt", "foo\nbar\nbaz\n");
}



void StatusModifiedFileTest::testModifiedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QVERIFY(file->idIndex() == "a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a");
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo().isNull());
	QVERIFY(file->path() == "modified.txt");
	QVERIFY(file->status() == Git::StatusFile::None);
}

void StatusModifiedFileTest::testModifiedFile_diffFiles()
{
	QVERIFY(status->diffFiles().size() == 1);

	Git::StatusFile *file = status->diffFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "modified.txt");
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void StatusModifiedFileTest::testModifiedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo() == "a907ec3f431eeb6b1c75799a7e4ba73ca6dc627a");
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex() == "100644");
	QVERIFY(file->modeRepo() == "100644");
	QVERIFY(file->path() == "modified.txt");
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void StatusModifiedFileTest::testModifiedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusModifiedFileTest::testModifiedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusModifiedFileTest::testModifiedFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("modified.txt").isEmpty());
}

void StatusModifiedFileTest::testModifiedFileIsModified()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("modified.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isModified());
	QVERIFY(file->status() == Git::StatusFile::Modified);
}

void StatusModifiedFileTest::testModifiedFileIsUnstaged()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(!file->isStaged());

	QVERIFY(!file->changesStaged());
	QVERIFY(file->changesUnstaged());
}

void StatusModifiedFileTest::testModifiedFileDefaultBlobIsFileBlob()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QVERIFY(file->blob() == file->blob("file"));
}

void StatusModifiedFileTest::testModifiedFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
}

void StatusModifiedFileTest::testModifiedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QVERIFY(file->blob("index") == "foo\nbar");
}

void StatusModifiedFileTest::testModifiedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QVERIFY(file->blob("repo") == "foo\nbar");
}

void StatusModifiedFileTest::testModifiedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->forFile("modified.txt")[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('modified.txt', d.a_path)
		assert_equal('modified.txt', d.b_path)
		assert(!d.new_file)
		assert(!d.deleted_file)
		assert_equal('a907ec3', d.a_sha)
		assert_equal('86e041d', d.b_sha)
		assert_equal("--- a/modified.txt\n+++ b/modified.txt\n@@ -1,2 +1,3 @@\n foo\n-bar\n\\ No newline at end of file\n+bar\n+baz\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QVERIFY("--- a/modified.txt\n+++ b/modified.txt\n@@ -1,2 +1,3 @@\n foo\n-bar\n\\ No newline at end of file\n+bar\n+baz\n");
}

#include "StatusModifiedFileTest.moc"
