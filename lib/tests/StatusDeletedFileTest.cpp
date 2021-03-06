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



class StatusDeletedFileTest : public GitTestBase
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

QTEST_KDEMAIN_CORE(StatusDeletedFileTest)



void StatusDeletedFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	cloneFrom("StatusDeletedFileTestRepo");
}



void StatusDeletedFileTest::testDeletedFile_lsFiles()
{
	QVERIFY(status->lsFiles().size() == 1);

	Git::StatusFile *file = status->lsFiles()[0];
	QCOMPARE(file->idIndex(), QString("86e041dad66a19b9518b83b78865015f62662f75"));
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QCOMPARE(file->modeIndex(), QString("100644"));
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("deleted.txt"));
	QCOMPARE(file->status(), Git::StatusFile::None);
}

void StatusDeletedFileTest::testDeletedFile_diffFiles()
{
	QVERIFY(status->diffFiles().size() == 1);

	Git::StatusFile *file = status->diffFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QCOMPARE(file->idRepo(), QString("86e041dad66a19b9518b83b78865015f62662f75"));
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QCOMPARE(file->modeRepo(), QString("100644"));
	QCOMPARE(file->path(), QString("deleted.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Deleted);
}

void StatusDeletedFileTest::testDeletedFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").size() == 1);

	Git::StatusFile *file = status->diffIndex("HEAD")[0];
	QVERIFY(file->idIndex().isNull());
	QCOMPARE(file->idRepo(), QString("86e041dad66a19b9518b83b78865015f62662f75"));
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QCOMPARE(file->modeRepo(), QString("100644"));
	QCOMPARE(file->path(), QString("deleted.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Deleted);
}

void StatusDeletedFileTest::testDeletedFile_diffUntrackedFiles()
{
	QVERIFY(status->untrackedFiles().isEmpty());
}

void StatusDeletedFileTest::testDeletedFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusDeletedFileTest::testDeletedFileHasStatus()
{
	QCOMPARE(status->files().size(), 1);
	QVERIFY(!status->forFile("deleted.txt").isEmpty());
}

void StatusDeletedFileTest::testDeletedFileIsDeleted()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("deleted.txt");
	QCOMPARE(fileStatus.size(), 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isDeleted());
	QCOMPARE(file->status(), Git::StatusFile::Deleted);
}

void StatusDeletedFileTest::testDeletedFileIsUnstaged()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(!file->isStaged());

	QVERIFY(!file->changesStaged());
	QVERIFY(file->changesUnstaged());
}

void StatusDeletedFileTest::testDeletedFileDefaultBlobIsFileBlob()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QCOMPARE(file->blob(), file->blob("file"));
}

void StatusDeletedFileTest::testDeletedFileFileBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QVERIFY(file->blob("file").isNull());
}

void StatusDeletedFileTest::testDeletedFileIndexBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QCOMPARE(file->blob("repo"), QByteArray("foo\nbar\nbaz\n", 12));
}

void StatusDeletedFileTest::testDeletedFileRepoBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("deleted.txt")[0];

	QCOMPARE(file->blob("repo"), QByteArray("foo\nbar\nbaz\n", 12));
}

void StatusDeletedFileTest::testDeletedFileDiffIsCorrect()
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
	QCOMPARE(diff, QString("--- a/deleted.txt\n+++ /dev/null\n@@ -1,3 +0,0 @@\n-foo\n-bar\n-baz\n"));
}

#include "StatusDeletedFileTest.moc"
