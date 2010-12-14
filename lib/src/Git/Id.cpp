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
	: QObject()
	, d(other.d)
{
}

Id::Id(const QString &sha1, Repo &repo)
	: QObject()
	, d(new IdPrivate)
{
	d->storage = repo.storageFor(sha1);

	if(d->storage) {
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
	: QObject()
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



int Id::compare(const Id &other) const
{
	return d->sha1.compare(other.d->sha1);
}

bool Id::exists() const
{
	return isValid() && d->storage && d->storage->contains(*this);
}

bool Id::isValid() const
{
	// construction should imply d->storage && ...
	return !d->sha1.isEmpty();
}

RawObject& Id::object() const
{
	if (!d->storage) {
		return RawObject::invalid();
	}

	return d->storage->objectFor(*this);
}

bool Id::operator<(const Id &other) const
{
	return d->sha1 < other.d->sha1;
}

Id& Id::operator=(const Id &other)
{
	Q_ASSERT(&other != this);

	d = other.d;

	return *this;
}

bool Id::operator==(const Id &other) const
{
	return d->sha1 == other.d->sha1;
}

bool Id::operator==(const QString &other) const
{
	return d->sha1 == other;
}

bool Id::operator!=(const Id &other) const
{
	return d->sha1 != other.d->sha1;
}

ObjectStorage& Id::storage() const
{
	return *d->storage;
}

const QByteArray Id::toBinarySha1() const
{
	return QByteArray::fromHex(d->sha1.toLatin1());
}

const QString& Id::toSha1String() const
{
	return d->sha1;
}

const QString Id::toShortSha1String() const
{
	return d->sha1.left(7);
}

const QString& Id::toString() const
{
	return toSha1String();
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
