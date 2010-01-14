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

void Repo::commitIndex(const QString &message, const QStringList &options)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	QStringList opts = options;
	opts << "-m" << message;

	runner.commit(opts);

	emit indexChanged();
	emit historyChanged();
}

QList<Commit*> Repo::commits(const QString &branch) const
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	QString result;
	if (runner.commits(branch) == DvcsJob::JobSucceeded) {
		result = runner.getResult();
	}

	return Commit::fromRawLog(this, result);
}

bool Repo::containsRepository(const QString &path)
{
	KUrl repoUrl(QDir(path).absolutePath());

	GitRunner runner;
	runner.setDirectory(repoUrl);

	return runner.isValidDirectory();
}

QString Repo::diff(const Commit &a, const Commit &b) const
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	runner.diffCommits(a.id(), b.id());

	return runner.getResult();
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

void Repo::stageFiles(const QStringList &paths)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	runner.add(paths, QStringList());

	emit indexChanged();
}

Status* Repo::status() const
{
	return new Status(this);
}

void Repo::unstageFiles(const QStringList &paths)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

/*
			commits = heads.inject(0) { |sum, head| sum + commit_count(head.commit.to_s) }
*/
	CommitList cs = commits();
	if (cs.isEmpty()) {
		runner.rm(paths, QStringList() << "--cached");
	} else {
		runner.reset(paths, QStringList(), "HEAD");
	}

	emit indexChanged();
}

const QString& Repo::workingDir() const
{
	return m_workingDir;
}

#include "Repo.moc"
