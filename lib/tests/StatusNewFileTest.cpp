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



class StatusNewFileTest : public GitTestBase
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

		void testNewFile_lsFiles();
		void testNewFile_diffFiles();
		void testNewFile_diffIndex();
		void testNewFile_diffUntrackedFiles();
		void testNewFile_diffIgnoredFiles();

		void testNewFileHasStatus();
		void testNewTestFileIsUntracked();
		void testNewFileIsUnstaged();
		void testNewFileDefaultBlobIsFileBlob();
		void testNewFileFileBlobIsCorrect();
		void testNewFileIndexBlobIsEmpty();
		void testNewFileRepoBlobIsEmpty();
		void testNewFileHasNoDiff();

	private:
		Git::Status *status;
};

QTEST_KDEMAIN_CORE(StatusNewFileTest)



void StatusNewFileTest::initTestCase()
{
	GitTestBase::initTestCase();
	status = 0;

	cloneFrom("StatusNewFileTestRepo");
}



void StatusNewFileTest::testNewFile_lsFiles()
{
	QVERIFY(status->lsFiles().isEmpty());
}

void StatusNewFileTest::testNewFile_diffFiles()
{
	QVERIFY(status->diffFiles().isEmpty());
}

void StatusNewFileTest::testNewFile_diffIndex()
{
	QVERIFY(status->diffIndex("HEAD").isEmpty());
}

void StatusNewFileTest::testNewFile_diffUntrackedFiles()
{
	QCOMPARE(status->untrackedFiles().size(), 1);

	Git::StatusFile *file = status->untrackedFiles()[0];
	QVERIFY(file->idIndex().isNull());
	QVERIFY(file->idRepo().isNull());
	QVERIFY(!file->isStaged());
	QVERIFY(file->modeIndex().isNull());
	QVERIFY(file->modeRepo().isNull());
	QCOMPARE(file->path(), QString("untracked.txt"));
	QCOMPARE(file->status(), Git::StatusFile::Untracked);
}

void StatusNewFileTest::testNewFile_diffIgnoredFiles()
{
	QVERIFY(status->ignoredFiles().isEmpty());
}



void StatusNewFileTest::testNewFileHasStatus()
{
	QCOMPARE(status->files().size(), 1);
	QVERIFY(!status->forFile("untracked.txt").isEmpty());
}

void StatusNewFileTest::testNewTestFileIsUntracked()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("untracked.txt");
	QCOMPARE(fileStatus.size(), 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isUntracked());
	QCOMPARE(file->status(), Git::StatusFile::Untracked);
}

void StatusNewFileTest::testNewFileIsUnstaged()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(!file->isStaged());

	QVERIFY(!file->changesStaged());
	QVERIFY(file->changesUnstaged());
}

void StatusNewFileTest::testNewFileDefaultBlobIsFileBlob()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QCOMPARE(file->blob(), file->blob("file"));
}

void StatusNewFileTest::testNewFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QCOMPARE(file->blob("file"), QByteArray("foo\nbar\nbaz\n", 12));
}

void StatusNewFileTest::testNewFileIndexBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob("index").isNull());
}

void StatusNewFileTest::testNewFileRepoBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob("repo").isNull());
}

void StatusNewFileTest::testNewFileHasNoDiff()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->diff().isNull());
}

#include "StatusNewFileTest.moc"
