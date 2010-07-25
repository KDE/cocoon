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

#include "Repo_p.h"

#include "gitrunner.h"
#include "Blob.h"
#include "Commit.h"
#include "Head.h"
#include "LooseStorage.h"
#include "PackedStorage.h"
#include "Status.h"
#include "Tree.h"

#include <KMessageBox>

#include <QDir>

using namespace Git;



Repo::Repo(const QString &workingDir, QObject *parent)
	: QObject(parent)
	, d(new RepoPrivate)
{
	d->gitDir = workingDir + "/.git";
	d->workingDir = workingDir;
	d->looseStorage = new LooseStorage(*this);
}

Repo::Repo(const Repo &other)
	: QObject(other.parent())
	, d(other.d)
{
}

Repo::~Repo()
{
	reset();
}



Blob* Repo::blob(const QString &id)
{
	RawObject *obj = object(id);

	return obj && obj->isBlob() ? static_cast<Blob*>(obj) : 0;
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

Commit* Repo::commit(const QString &id)
{
	RawObject *obj = object(id);

	return obj && obj->isCommit() ? static_cast<Commit*>(obj) : 0;
}

CommitList Repo::commits(const QString &branch)
{
	if (!d->commits.contains(branch)) {
		d->commits[branch] = Commit::allReachableFrom(*head(branch));
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

	runner.diffCommits(a.id().toSha1String(), b.id().toSha1String());

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

Ref* Repo::head(const QString &head)
{
	QString name = head;

	if (name.isEmpty()) {
		QFile head(gitDir() + "/HEAD");
		//kDebug() << "reading HEAD:" << head.fileName();
		head.open(QFile::ReadOnly);

		/** @todo make it recognize any type of ref */
		name = head.readAll().split('/').last().trimmed();

		//kDebug() << "found head named:" << name;
		head.close();
	}

	return new Head(name, *this);
}

RefList Repo::heads()
{
	if (d->heads.isEmpty()) {
		d->heads = Head(*this).all();
	}

	return d->heads;
}

RawObject* Repo::object(const QString &id)
{
	ObjectStorage *storage = storageFor(id);
	return storage ? storage->objectFor(Id(id, *storage)) : 0;
}

void Repo::reset()
{
	resetCommits();
	resetHeads();
	resetStatus();
	resetStorages();
}

void Repo::resetCommits()
{
	if (!d->commits.isEmpty()) {
		d->commits.clear();

		emit indexChanged();
	}
}

void Repo::resetHeads()
{
	if (!d->heads.isEmpty()) {
		foreach (Ref *ref, heads()) {
			delete ref;
		}
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

void Repo::resetStorages()
{
	if (!d->storages.isEmpty()) {
		foreach (ObjectStorage *storage, d->storages) {
			storage->deleteLater();
		}
	}
	d->storages.clear();


	if (d->looseStorage) {
		d->looseStorage->deleteLater();
		d->looseStorage = 0;
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

ObjectStorage* Repo::storageFor(const QString &id)
{
	foreach(ObjectStorage *storage, storages()) {
		if (storage->contains(id)) {
			return storage;
		}
	}

	/** @todo throw exception */
	return 0;
}

const QList<ObjectStorage*> Repo::storages()
{
	if (d->storages.isEmpty()) {

		d->storages << d->looseStorage;
		foreach(const QString &name, PackedStorage::allNamesIn(*this)) {
			d->storages << new PackedStorage(name, *this);
		}
	}

	return d->storages;
}

Tree* Repo::tree(const QString &id)
{
	RawObject *obj = object(id);

	return obj && obj->isTree() ? static_cast<Tree*>(obj) : 0;
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
