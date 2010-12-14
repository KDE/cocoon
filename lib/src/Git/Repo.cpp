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
	disconnect();
	reset();
}



Blob Repo::blob(const Id &id)
{
	return id.object().toBlob();
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
	resetRefs();
	resetLooseStorage();
	resetStatus();

	emit headsChanged();
	emit indexChanged();
	emit historyChanged();
}

Commit Repo::commit(const Id &id)
{
	return id.object().toCommit();
}

QList<Commit> Repo::commits(const QString &branch)
{
	Ref branchRef = ref(branch);
	if (!d->commits.contains(branchRef.fullName())) {
		d->commits[branchRef.fullName()] = Commit::allReachableFrom(branchRef);
	}

	return d->commits[branchRef.fullName()];
}

bool Repo::containsRepository(const QString &path)
{
	KUrl repoUrl(QDir(path).absolutePath());

	GitRunner runner;
	runner.setDirectory(repoUrl);

	return runner.isValidDirectory();
}

const Ref& Repo::currentHead()
{
	QString head("HEAD");

	if (!d->refs.contains(head)) {
		QFile headFile(gitDir() + "/HEAD");
		kDebug() << "reading HEAD:" << headFile.fileName();

		headFile.open(QFile::ReadOnly);
		QString headContent = headFile.readAll();
		headFile.close();

		Q_ASSERT(headContent.startsWith("ref:"));
		QString ref = headContent.mid(5).trimmed(); // 5 == "ref: ".length
		kDebug() << "HEAD points at:" << ref;

		d->refs[head] = this->ref(ref);
	}

	return d->refs[head];
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

QList<Ref> Repo::heads()
{
	QList<Ref> refs;

	if (d->refs.isEmpty()) {
		foreach (const Ref &head, Head(*this).all()) {
			d->refs[head.fullName()] = head;
		}
	}

	foreach (const Ref &ref, d->refs.values()) {
		if (!ref.isRemote() && ref.prefix() == "heads") {
			refs << ref;
		}
	}

	return refs;
}

Id Repo::idFor(const QString &idString) {
	return Id(idString, *this);
}

RawObject& Repo::object(const Id &id)
{
	return id.object();
}

const Ref& Repo::ref(const QString &name)
{
	if (name.isEmpty() || name == "HEAD") {
		return currentHead();
	}

	QString fullName = Ref::fullNameFor(name, *this);
	Ref ref = d->refs[fullName]; // ref is invalid if it has not been loaded yet

	if (!ref.isValid() && !fullName.isEmpty()) {
		QStringList parts = fullName.split("/");
		Q_ASSERT(parts[0] == "refs");
		parts.removeFirst();

		if (parts.size() == 2) {
			ref = Ref::newInstance(QString(), parts[0], parts[1], *this);
		} else {
			Q_ASSERT(parts.size() == 3);
			ref = Ref::newInstance(parts[0], parts[1], parts[2], *this);
		}

		d->refs[fullName] = ref;
	}

	return d->refs[fullName];
}

void Repo::reset()
{
	resetCommits();
	resetRefs();
	resetLooseStorage();
	resetPackedStorages();
	resetStatus();

	emit headsChanged();
	emit historyChanged();
	emit indexChanged();
}

void Repo::resetCommits()
{
	if (!d->commits.isEmpty()) {
		d->commits.clear();
	}
}

void Repo::resetRefs()
{
	if (!d->refs.isEmpty()) {
		d->refs.clear();
	}
}

void Repo::resetStatus()
{
	/** @todo make Status smarter to detect only changed files */
	if (d->status) {
		delete d->status;
		d->status = 0;
	}
}

void Repo::resetLooseStorage()
{
	d->looseStorage->reset();
}

void Repo::resetPackedStorages()
{
	if (!d->storages.isEmpty()) {
		foreach (ObjectStorage *storage, d->storages) {
			storage->deleteLater();
		}
	}
	d->storages.clear();
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

Tree Repo::tree(const Id &id)
{
	return id.object().toTree();
}

void Repo::unstageFiles(const QStringList &paths)
{
	GitRunner runner;
	runner.setDirectory(workingDir());

/*
			commits = heads.inject(0) { |sum, head| sum + commit_count(head.commit.to_s) }
*/
	QList<Commit> cs = commits();
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
