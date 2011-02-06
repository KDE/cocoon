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

#include "Ref.h"
#include "Ref_p.h"

#include "Repo.h"

#include <KDebug>

using namespace Git;



Ref::Ref()
	: QObject()
	, d(new RefPrivate)
{
}

Ref::Ref(const Ref &other)
	: QObject()
	, d(other.d)
{
}

Ref::Ref(Repo &repo)
	: QObject()
	, d(new RefPrivate)
{
	d->repo = &repo;
	d->refsDir = QDir(d->repo->gitDir());
	d->refsDir.cd("refs");
}

Ref::Ref(const QString &remote, const QString &prefix, const QString &name, Repo &repo)
	: QObject(&repo)
	, d(new RefPrivate)
{
	d->remote = remote;
	d->prefix = prefix;
	d->name = name;
	d->repo = &repo;
	d->refsDir = QDir(d->repo->gitDir());
	d->refsDir.cd("refs");
	d->refsDir.cd(remote);
	d->refsDir.cd(prefix);

	populate();
}



QList<Ref> Ref::all(const QString &remote, const QString &prefix) const
{
	QList<Ref> refs;

	QDir refsDir = QDir(d->repo->gitDir());
	refsDir.cd("refs");
	refsDir.cd(remote);
	refsDir.cd(prefix);

	foreach (const QString &name, refsDir.entryList(QDir::Files)) {
		//kDebug() << "ref found:" << name;
		refs << Ref(remote, prefix, name, *d->repo);
	}

	return refs;
}

QList<Ref> Ref::allHeads() const
{
	return all(QString(), "heads");
}

const QString& Ref::name() const
{
	return d->name;
}

Commit Ref::commit() const
{
	return d->commitId.object().toCommit();
}

bool Ref::exists(const QString &name, const Repo &repo)
{
	return QFile::exists(QDir(repo.gitDir()).filePath(name));
}

const QString Ref::fullName() const
{
	if (!isValid()) {
		return QString();
	}

	if (isRemote()) {
		return QString("refs/%3/%2/%1").arg(name()).arg(prefix()).arg(remote());
	} else {
		return QString("refs/%2/%1").arg(name()).arg(prefix());
	}
}

QString Ref::fullNameFor(const QString &name, const Repo &repo)
{
	// see: http://www.kernel.org/pub/software/scm/git/docs/git-rev-parse.html#_specifying_revisions
	QStringList searchPaths;
	searchPaths << "%1" << "refs/%1" << "refs/tags/%1" << "refs/heads/%1" << "refs/remotes/%1" << "refs/remotes/%1/HEAD";

	foreach (QString path, searchPaths) {
		path = path.arg(name);
		if (exists(path, repo)) {
			kDebug() << "Full ref name for" << name << "is" << path;
			return path;
		}
	}

	return QString();
}

Ref Ref::head(const QString &name, Repo &repo)
{
	return Ref(QString(), "heads", name, repo);
}

bool Ref::isRemote() const
{
	return !remote().isEmpty();
}

bool Ref::isValid() const
{
	return !name().isEmpty();
}

Ref& Ref::operator=(const Ref &other)
{
	setParent(other.parent());

	d = other.d;

	return *this;
}

bool Ref::operator==(const Ref &other) const
{
	return other.d->name == d->name && other.d->prefix == d->prefix;
}

void Ref::populate()
{
	QFile refFile(d->refsDir.filePath(d->name));
	refFile.open(QFile::ReadOnly);

	QString commitId = refFile.readAll().trimmed();

	//kDebug() << "reading head:" << refFile.fileName();
	//kDebug() << "head content:" << commitId;

	d->commitId = d->repo->idFor(commitId);

	refFile.close();
}

const QString& Ref::prefix() const
{
	return d->prefix;
}

const QString& Ref::remote() const
{
	return d->remote;
}

Repo& Ref::repo() const
{
	return *d->repo;
}



#include "Ref.moc"
