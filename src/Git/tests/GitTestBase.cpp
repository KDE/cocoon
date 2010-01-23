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



void GitTestBase::deleteFile(const QString &file)
{
	QDir().remove(pathTo(file));
}

QStringList GitTestBase::gitBasicOpts()
{
	QStringList opts;

	opts << QString("--git-dir=%1/.git").arg(workingDir);
	opts << QString("--work-tree=%1").arg(workingDir);

	return opts;
}

QString GitTestBase::pathTo(const QString &file)
{
	return QDir(workingDir).filePath(file);
}

void GitTestBase::writeToFile(const QString &filePath, const QByteArray &content)
{
	QFile file(pathTo(filePath));
	file.open(QFile::ReadWrite);
	file.write(content);
	file.close();
}



void GitTestBase::initTestCase()
{
	repo = 0;

	srand(time(0));
	workingDir = QDir::temp().filePath("git_test_%1_%2").arg(QDateTime::currentDateTime().toTime_t()).arg(rand());
	qDebug() << "Test directory: " << workingDir;

	QProcess::execute("mkdir", QStringList() << workingDir);
	QProcess::execute("git", gitBasicOpts() << "init");
}

void GitTestBase::cleanupTestCase()
{
	QProcess::execute("rm", QStringList() << "-rf" << workingDir);
}

void GitTestBase::init()
{
	repo = new Git::Repo(workingDir, this);
}

void GitTestBase::cleanup()
{
	repo->reset();
	delete repo;
}

#include "GitTestBase.moc"
