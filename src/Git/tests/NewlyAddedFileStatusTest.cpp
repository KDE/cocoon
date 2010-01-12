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

#include <QObject>

#include "Git/Repo.h"
#include "Git/Status.h"

#include <QtTest/QtTest>
#include <qtest_kde.h>

#include <QProcess>



class NewlyAddedFileStatusTest : public QObject
{
	Q_OBJECT

	private:
		QStringList gitBasicOpts();

	private slots:
		void initTestCase();
		void cleanupTestCase();
		void init();
		void cleanup();

		void testNewlyAddedFileHasStatus();
		void testNewlyAddedFileIsAdded();
		void testNewlyAddedFileIsStaged();
		void testNewlyAddedFileDefaultBlobIsIndexBlob();
		void testNewlyAddedFileFileBlobIsCorrect();
		void testNewlyAddedFileIndexBlobIsCorrect();
		void testNewlyAddedFileRepoBlobIsEmpty();
		void testNewlyAddedFileDiffIsCorrect();

	private:
		Git::Repo *repo;
		Git::Status *status;
		QString workingDir;
};

QTEST_KDEMAIN_CORE(NewlyAddedFileStatusTest)



QStringList NewlyAddedFileStatusTest::gitBasicOpts()
{
	QStringList opts;

	opts << QString("--git-dir=%1/.git").arg(workingDir);
	opts << QString("--work-tree=%1").arg(workingDir);

	return opts;
}

void NewlyAddedFileStatusTest::initTestCase()
{
	repo = 0;
	status = 0;
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

void NewlyAddedFileStatusTest::cleanupTestCase()
{
	QProcess::execute("rm", QStringList() << "-rf" << workingDir);
}

void NewlyAddedFileStatusTest::init()
{
	repo = new Git::Repo(workingDir, this);
	status = repo->status();
}

void NewlyAddedFileStatusTest::cleanup()
{
	delete status;
	delete repo;
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
