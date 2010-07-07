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

#include "Id.h"

#include "ObjectStorage.h"
#include "Repo.h"

using namespace Git;



Id::Id(const QString &sha1, Repo &repo)
	: QObject(&repo)
	, d(new IdPrivate)
{
	d->repo = &repo;

	if (sha1.size() == 40) {
		d->sha1 = sha1;
	} else {
		foreach(ObjectStorage *storage, d->repo->storages()) {
			QString actualId = storage->actualIdFor(sha1);
			if (!actualId.isEmpty()) {
				d->sha1 = actualId;
				break;
			}
		}
	}
}

Id::Id(const Id &other)
	: QObject(d->repo)
	, d(other.d)
{
}

Id::Id()
	: QObject()
	, d(new IdPrivate)
{
}



Id& Id::operator=(const Id &other)
{
	setParent(other.parent());
	d = other.d;

	return *this;
}

bool Id::operator==(const Id &other) const
{
	return d->sha1 == other.d->sha1 && d->repo == other.d->repo;
}

const QString& Id::sha1() const
{
	return d->sha1;
}

const QString& Id::toString() const
{
	return d->sha1;
}



QDataStream& operator<<(QDataStream &stream, const Id &id)
{
	stream << id.toString();
	return stream;
}

uint qHash(const Id &id)
{
	return id.sha1().left(2*sizeof(uint)).toUInt(0, 16);
}

#include "Id.moc"
