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



class StatusModifiedAddedFileTest : public GitTestBase
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

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusModifiedAddedFileTest)



void StatusModifiedAddedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	cloneFrom("StatusModifiedAddedFileTestRepo");
}



void StatusModifiedAddedFileTest::testRemodifiedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QCOMPARE(file->idIndex(), QString("19102815663d23f8b75a47e7a01965dcdc96468c"));
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("modified_added.txt"));
	QCOMPARE(file->status(), Git::StatusFile::None);
}

void StatusModifiedAddedFileTest::testRemodifiedFile_diffFiles()
{
	QVERIFY(status->diffFiles().size() == 1);

	Git::StatusFile *file = status->diffFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QCOMPARE(file->idRepo(), QString("19102815663d23f8b75a47e7a01965dcdc96468c"));
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QCOMPARE(file->modeRepo(), QString("100644"));
	QCOMPARE(file->path(), QString("modified_added.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Modified);
}

void StatusModifiedAddedFileTest::testRemodifiedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("modified_added.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Added);
}

void StatusModifiedAddedFileTest::testRemodifiedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusModifiedAddedFileTest::testRemodifiedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusModifiedAddedFileTest::testRemodifiedFileHasStatus()
{
	QCOMPARE(status->files().size(), 2);
	QVERIFY(!status->forFile("modified_added.txt").isEmpty());
}

void StatusModifiedAddedFileTest::testRemodifiedFileIsModified()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("modified_added.txt");
	QCOMPARE(fileStatus.size(), 2);

	Git::StatusFile *file = 0;

	file = fileStatus[0];
	QVERIFY(file->isModified());
	QCOMPARE(file->status(), Git::StatusFile::Modified);

	file = fileStatus[1];
	QVERIFY(file->isAdded());
	QCOMPARE(file->status(), Git::StatusFile::Added);
}

void StatusModifiedAddedFileTest::testRemodifiedFileIsStagedAndUnstaged()
{
	Git::StatusFile *file = 0;

	file = status->forFile("modified_added.txt")[0];
	QVERIFY(file->changesUnstaged());

	file = status->forFile("modified_added.txt")[1];
	QVERIFY(file->changesStaged());
}

void StatusModifiedAddedFileTest::testUnstagedRemodifiedFileBlobsAreCorrect()
{
	Git::StatusFile *file = status->unstagedFiles()[0];

	QCOMPARE(file->blob(), file->blob("file"));
	QCOMPARE(file->blob("file"), QByteArray("foo\nbar\n", 8));
	QCOMPARE(file->blob("index"), QByteArray("foo", 3));
	QCOMPARE(file->blob("repo"), QByteArray("foo", 3));
}

void StatusModifiedAddedFileTest::testStagedRemodifiedFileBlobsAreCorrect()
{
	Git::StatusFile *file = status->stagedFiles()[0];

	QCOMPARE(file->blob(), file->blob("index"));
	QCOMPARE(file->blob("file"), QByteArray("foo\nbar\n", 8));
	QVERIFY(file->blob("index").isNull());
	QVERIFY(file->blob("repo").isNull());
}

void StatusModifiedAddedFileTest::testUnstagedRemodifiedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->unstagedFiles()[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('modified_added.txt', d.a_path)
		assert_equal('modified_added.txt', d.b_path)
		assert(!d.new_file)
		assert(!d.deleted_file)
		assert_equal('1910281', d.a_sha)
		assert_equal('3bd1f0e', d.b_sha)
		assert_equal("--- a/modified_added.txt\n+++ b/modified_added.txt\n@@ -1 +1,2 @@\n-foo\n\\ No newline at end of file\n+foo\n+bar\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QCOMPARE(diff, QString("--- a/modified_added.txt\n+++ b/modified_added.txt\n@@ -1 +1,2 @@\n-foo\n\\ No newline at end of file\n+foo\n+bar\n"));
}

void StatusModifiedAddedFileTest::testStagedRemodifiedFileDiffIsCorrect()
{
	Git::StatusFile *file = status->stagedFiles()[0];

	QString diff = file->diff();

/*
		assert_not_nil(d)
		assert_equal('modified_added.txt', d.a_path)
		assert_equal('modified_added.txt', d.b_path)
		assert(d.new_file)
		assert(!d.deleted_file)
		assert_nil(d.a_sha)
		assert_equal('1910281', d.b_sha)
		assert_equal("--- /dev/null\n+++ b/modified_added.txt\n@@ -0,0 +1 @@\n+foo\n\\ No newline at end of file\n", d.diff)
*/
	QVERIFY(!diff.isNull());
	QCOMPARE(diff, QString("--- /dev/null\n+++ b/modified_added.txt\n@@ -0,0 +1 @@\n+foo\n\\ No newline at end of file\n"));
}

#include "StatusModifiedAddedFileTest.moc"
