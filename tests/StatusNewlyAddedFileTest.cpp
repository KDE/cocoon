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



class StatusNewlyAddedFileTest : public GitTestBase
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

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusNewlyAddedFileTest)



void StatusNewlyAddedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	QProcess::execute("mkdir", QStringList() << "-p" << workingDir);
	QProcess::execute("git", gitBasicOpts() << "init");

	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	writeToFile("newly_added.txt", "foo\nbar\nbaz\n");

	QProcess::execute("git", gitBasicOpts() << "add" << "newly_added.txt");
}



void StatusNewlyAddedFileTest::testNewlyAddedFile_lsFiles()
{
	QCOMPARE(status->lsFiles().size(), 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QCOMPARE(file->idIndex(), QString("86e041dad66a19b9518b83b78865015f62662f75"));
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("newly_added.txt"));
	QCOMPARE(file->status(), Git::StatusFile::None);
}

void StatusNewlyAddedFileTest::testNewlyAddedFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void StatusNewlyAddedFileTest::testNewlyAddedFile_diffIndex()
{
	QCOMPARE(status->diffIndex("HEAD").size(), 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QCOMPARE(file->idIndex(), QString("86e041dad66a19b9518b83b78865015f62662f75"));
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("newly_added.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Added);
}

void StatusNewlyAddedFileTest::testNewlyAddedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusNewlyAddedFileTest::testNewlyAddedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusNewlyAddedFileTest::testNewlyAddedFileHasStatus()
{
	QCOMPARE(status->files().size(), 1);
	QVERIFY(!status->forFile("newly_added.txt").isEmpty());
}

void StatusNewlyAddedFileTest::testNewlyAddedFileIsAdded()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("newly_added.txt");
	QCOMPARE(fileStatus.size(), 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isAdded());
	QCOMPARE(file->status(), Git::StatusFile::Added);
}

void StatusNewlyAddedFileTest::testNewlyAddedFileIsStaged()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(file->isStaged());

	QVERIFY(file->changesStaged());
	QVERIFY(!file->changesUnstaged());
}

void StatusNewlyAddedFileTest::testNewlyAddedFileDefaultBlobIsIndexBlob()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QCOMPARE(file->blob(), file->blob("index"));
}

void StatusNewlyAddedFileTest::testNewlyAddedFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QCOMPARE(file->blob("file"), QByteArray("foo\nbar\nbaz\n", 12));
}

void StatusNewlyAddedFileTest::testNewlyAddedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QCOMPARE(file->blob("index"), QByteArray("foo\nbar\nbaz\n", 12));
}

void StatusNewlyAddedFileTest::testNewlyAddedFileRepoBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("newly_added.txt")[0];

	QVERIFY(file->blob("repo").isNull());
}

void StatusNewlyAddedFileTest::testNewlyAddedFileDiffIsCorrect()
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
	QVERIFY(diff == "--- /dev/null\n+++ b/newly_added.txt\n@@ -0,0 +1,3 @@\n+foo\n+bar\n+baz\n");
}

#include "StatusNewlyAddedFileTest.moc"
