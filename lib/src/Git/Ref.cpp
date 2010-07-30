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

#include <KDebug>

using namespace Git;



Ref::Ref(const Ref &other)
	: QObject(other.parent())
	, d(other.d)
{
}

Ref::Ref(const QString &prefix, Repo &repo)
	: QObject(&repo)
	, d(new RefPrivate)
{
	d->repo = &repo;
	d->refsDir = QDir(d->repo->gitDir());
	d->refsDir.cd("refs");
	d->refsDir.cd(prefix);
}

Ref::Ref(const QString &name, const QString &prefix, Repo &repo)
	: QObject(&repo)
	, d(new RefPrivate)
{
	d->name = name;
	d->repo = &repo;
	d->refsDir = QDir(d->repo->gitDir());
	d->refsDir.cd("refs");
	d->refsDir.cd(prefix);

	populate();
}



RefList Ref::all() const
{
	RefList refs;

	foreach (const QString &name, d->refsDir.entryList(QDir::Files)) {
		//kDebug() << "ref found:" << name;
		refs << newInstance(name, *d->repo);
	}

	return refs;
}

const QString& Ref::name() const
{
	return d->name;
}

Commit* Ref::commit() const
{
	return d->repo->commit(d->commitId.toSha1String());
}

Ref& Ref::operator=(const Ref &other)
{
	d = other.d;

	return *this;
}

void Ref::populate()
{
	QFile refFile(d->refsDir.filePath(d->name));
	refFile.open(QFile::ReadOnly);

	QString commitId = refFile.readAll().trimmed();

	//kDebug() << "reading head:" << refFile.fileName();
	//kDebug() << "head content:" << commitId;

	d->commitId = Id(commitId, *d->repo);

	refFile.close();
}



#include "Ref.moc"
