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



class NewFileStatusTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase();

		void testNewFileHasStatus();
		void testNewTestFileIsUntracked();
		void testNewFileIsUnstaged();
		void testNewFileDefaultBlobIsFileBlob();
		void testNewFileFileBlobIsCorrect();
		void testNewFileIndexBlobIsEmpty();
		void testNewFileRepoBlobIsEmpty();
		void testNewFileHasNoDiff();
};

QTEST_KDEMAIN_CORE(NewFileStatusTest)



void NewFileStatusTest::initTestCase()
{
	GitTestBase::initTestCase();

	workingDir = QDir::temp().filePath("new_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	qDebug() << workingDir;

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("untracked.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();
}



void NewFileStatusTest::testNewFileHasStatus()
{
	QVERIFY(status->files().size() == 1);
	QVERIFY(!status->forFile("untracked.txt").isEmpty());
}

void NewFileStatusTest::testNewTestFileIsUntracked()
{
	QList<Git::StatusFile*> fileStatus = status->forFile("untracked.txt");
	QVERIFY(fileStatus.size() == 1);

	Git::StatusFile *file = fileStatus[0];
	QVERIFY(file->isUntracked());
	QVERIFY(file->status() == "U");
}

void NewFileStatusTest::testNewFileIsUnstaged()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->hasChanged());
	QVERIFY(!file->isStaged());

	QVERIFY(!file->changesStaged());
	QVERIFY(file->changesUnstaged());
}

void NewFileStatusTest::testNewFileDefaultBlobIsFileBlob()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob() == file->blob("file"));
}

void NewFileStatusTest::testNewFileFileBlobIsCorrect()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob("file") == "foo\nbar\nbaz\n");
}

void NewFileStatusTest::testNewFileIndexBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob("index").isNull());
}

void NewFileStatusTest::testNewFileRepoBlobIsEmpty()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->blob("repo").isNull());
}

void NewFileStatusTest::testNewFileHasNoDiff()
{
	Git::StatusFile *file = status->forFile("untracked.txt")[0];

	QVERIFY(file->diff().isNull());
}

#include "NewFileStatusTest.moc"
