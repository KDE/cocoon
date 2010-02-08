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
#include "Commit.h"
#include "Head.h"
#include "LooseStorage.h"
#include "Status.h"

#include <KMessageBox>

#include <QDir>

using namespace Git;



Repo::Repo(const QString &workingDir, QObject *parent)
	: QObject(parent)
{
	d = new RepoPrivate(*this);
	d->gitDir = workingDir + "/.git";
	d->workingDir = workingDir;
}

Repo::Repo(const Repo &other)
	: QObject(other.parent())
	, d(other.d)
{
}

Repo::~Repo()
{
}



void Repo::clone(const QString &fromRepo, const QString &toDirectory, const QStringList &options)
{
	GitRunner runner;
	runner.setDirectory(toDirectory);

	runner.clone(fromRepo, options, toDirectory);
}

void Repo::commitIndex(const QString &message, const QStringList &options)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	QStringList opts = options;
	opts << "-m" << message;

	runner.commit(opts);

	resetCommits();
	resetStatus();

	emit indexChanged();
	emit historyChanged();
}

Commit* Repo::commit(const QString &id) const
{
	RawObject *obj = ((LooseStorage)d->looseStorage).rawObjectFor(id);

	return obj->isCommit() ? static_cast<Commit*>(obj) : 0;
}

CommitList Repo::commits(const QString &branch)
{
	if (!d->commits.contains(branch)) {
		if (branch.isEmpty()) {
			d->commits[branch] = Commit::allReachableFrom(*head());
		} else {
			d->commits[branch] = Commit::allReachableFrom(Head(branch, *this));
		}
	}

	return d->commits[branch];
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

const QString& Repo::gitDir() const
{
	return d->gitDir;
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

Ref* Repo::head() const
{
	QFile head(gitDir() + "/HEAD");
	kDebug() << "reading HEAD:" << head.fileName();
	head.open(QFile::ReadOnly);
	/** @todo make it recognize any type of ref */
	QString name = head.readAll().split('/').last().trimmed();
	kDebug() << "found head named:" << name;
	head.close();

	return new Head(name, *this);
}

RefList Repo::heads()
{
	if (d->heads.isEmpty()) {
		d->heads = Head(*this).all();
	}

	return d->heads;
}

void Repo::reset()
{
	resetCommits();
	resetHeads();
	resetStatus();
}

void Repo::resetCommits()
{
	if (d->commits.size() > 0) {
		d->commits.clear();

		emit indexChanged();
	}
}

void Repo::resetHeads()
{
	if (!d->heads.isEmpty()) {
		d->heads.clear();

		emit headsChanged();
	}
}

void Repo::resetStatus()
{
	/** @todo make Status smarter to detect only changed files */
	if (d->status) {
		delete d->status;
		d->status = 0;

		emit indexChanged();
	}
}

void Repo::stageFiles(const QStringList &paths)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

	runner.add(paths, QStringList());

	resetStatus();
	emit indexChanged();
}

Status* Repo::status()
{
	if (!d->status) {
		d->status = new Status(this);
	}

	return d->status;
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

	resetStatus();
	emit indexChanged();
}

const QString& Repo::workingDir() const
{
	return d->workingDir;
}

#include "Repo.moc"
