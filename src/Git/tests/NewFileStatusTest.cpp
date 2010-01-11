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



class NewFileStatusTest : public QObject
{
	Q_OBJECT

	private:
		QStringList gitBasicOpts();

	private slots:
		void initTestCase();
		void cleanupTestCase();
		void init();
		void cleanup();

		void testNewFileHasStatus();
		void testNewTestFileIsUntracked();

	private:
		Git::Repo *repo;
		Git::Status *status;
		QString workingDir;
};

QTEST_KDEMAIN_CORE(NewFileStatusTest)



QStringList NewFileStatusTest::gitBasicOpts()
{
	QStringList opts;

	opts << QString("--git-dir=%1/.git").arg(workingDir);
	opts << QString("--work-tree=%1").arg(workingDir);

	return opts;
}

void NewFileStatusTest::initTestCase()
{
	repo = 0;
	status = 0;
	workingDir = QDir::temp().filePath("new_file_status_test_repo");

	QProcess::execute("mkdir", QStringList() << workingDir);

	QProcess::execute("git", gitBasicOpts() << "init");
	QProcess::execute("git", gitBasicOpts() << "commit" << "--allow-empty" << "-m" << "Empty inital commit.");

	QFile file(QDir(workingDir).filePath("untracked.txt"));
	file.open(QFile::ReadWrite);
	file.write("foo\nbar\nbaz\n");
	file.close();
}

void NewFileStatusTest::cleanupTestCase()
{
	QProcess::execute("rm", QStringList() << "-rf" << workingDir);
}

void NewFileStatusTest::init()
{
	repo = new Git::Repo(workingDir, this);
	status = repo->status();
}

void NewFileStatusTest::cleanup()
{
	delete status;
	delete repo;
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

#include "NewFileStatusTest.moc"
