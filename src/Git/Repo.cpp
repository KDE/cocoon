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

#include "Repo.h"

#include "gitrunner.h"

#include <KMessageBox>

#include <QDir>

using namespace Git;



Repo::Repo(const QString &workingDir, QObject *parent)
	: QObject(parent)
	, m_workingDir(workingDir)
{
}

QList<Commit*> Repo::commits() const
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	QStringList lines;
	if (runner.commits() == DvcsJob::JobSucceeded) {
		lines = runner.getResult().split("\n");
	}

	return Commit::fromRawRevList(this, lines);
}

bool Repo::containsRepository(const QString &path)
{
	KUrl repoUrl(QDir(path).absolutePath());

	GitRunner runner;
	runner.setDirectory(repoUrl);

	return runner.isValidDirectory();
}

void Repo::init(const QString &newRepoPath)
{
	KUrl newRepoUrl(QDir(newRepoPath).absolutePath());

	GitRunner runner;
	runner.setDirectory(newRepoUrl);

	if (runner.init(newRepoUrl) != DvcsJob::JobSucceeded) {
		KMessageBox::error(0, runner.getResult(), i18n("Git Error"));
	}
}

QString Repo::head() const
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	if (runner.currentBranch() == DvcsJob::JobSucceeded) {
		return runner.getResult();
	}

	return QString();
}

QStringList Repo::heads() const
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	runner.branches();

	QStringList branches = runner.getResult().split("\n");
	branches.removeLast(); // splitting results in empty last line
	for(int i=0; i < branches.size(); ++i) {
		branches[i] = branches[i].mid(2);
	}

	return branches;
}

const QString& Repo::workingDir() const
{
	return m_workingDir;
}

#include "Repo.moc"
