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



QStringList GitTestBase::gitBasicOpts()
{
	QStringList opts;

	opts << QString("--git-dir=%1/.git").arg(workingDir);
	opts << QString("--work-tree=%1").arg(workingDir);

	return opts;
}

void GitTestBase::initTestCase()
{
	repo = 0;
	status = 0;
}

void GitTestBase::cleanupTestCase()
{
	QProcess::execute("rm", QStringList() << "-rf" << workingDir);
}

void GitTestBase::init()
{
	repo = new Git::Repo(workingDir, this);
	status = repo->status();
}

void GitTestBase::cleanup()
{
	delete status;
	delete repo;
}

#include "GitTestBase.moc"
