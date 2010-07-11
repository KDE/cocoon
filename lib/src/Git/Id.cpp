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
#include "Id_p.h"

#include "ObjectStorage.h"
#include "Repo.h"

using namespace Git;



Id::Id()
	: QObject()
	, d(new IdPrivate)
{
}

Id::Id(const Id &other)
	: QObject(other.d->storage)
	, d(other.d)
{
}

Id::Id(const QString &sha1, Repo &repo)
	: QObject(&repo)
	, d(new IdPrivate)
{
	ObjectStorage *storage = repo.storageFor(sha1);
	if(storage) {
		d->storage = storage;
		setParent(storage);

		if (sha1.size() == 40) {
			d->sha1 = sha1;
		} else {
			QString actualId = d->storage->actualIdFor(sha1);
			if (!actualId.isEmpty()) {
				d->sha1 = actualId;
			}
		}
	}
}

Id::Id(const QString &sha1, ObjectStorage &storage)
	: QObject(&storage)
	, d(new IdPrivate)
{
	d->storage = &storage;

	if (sha1.size() == 40) {
		d->sha1 = sha1;
	} else {
		QString actualId = d->storage->actualIdFor(sha1);
		if (!actualId.isEmpty()) {
			d->sha1 = actualId;
		}
	}
}

Id::~Id()
{
}



bool Id::exists() const
{
	return isValid() && d->storage->contains(d->sha1);
}

bool Id::isValid() const
{
	return d->storage;
}

Id& Id::operator=(const Id &other)
{
	Q_ASSERT(&other != this);

	setParent(other.parent());
	d = other.d;

	return *this;
}

bool Id::operator==(const Id &other) const
{
	return d->sha1 == other.d->sha1 && d->storage == other.d->storage;
}

ObjectStorage& Id::storage() const
{
	return *d->storage;
}

const QByteArray Id::toBinarySha1() const
{
	return d->sha1.toLatin1().toHex();
}

const QString& Id::toSha1String() const
{
	return d->sha1;
}

const QString Id::toShortSha1String() const
{
	return d->sha1.left(7);
}

const QString Id::toString() const
{
	return toShortSha1String();
}



QDataStream& operator<<(QDataStream &stream, const Id &id)
{
	stream << id.toSha1String();
	return stream;
}

uint qHash(const Id &id)
{
	return id.toSha1String().left(2*sizeof(uint)).toUInt(0, 16);
}

#include "Id.moc"
