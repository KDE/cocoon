/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#include "RawObject.h"
#include "RawObject_p.h"

#include "Blob.h"
#include "Commit.h"
#include "ObjectStorage.h"
#include "Repo.h"
#include "Tree.h"

#include <KDebug>

#include <QRegExp>

using namespace Git;



RawObject::RawObject()
	: QObject()
	, d(new RawObjectPrivate)
{
}

RawObject::RawObject(const Id &id, Repo &repo)
	: QObject((QObject*)&repo)
	, d(new RawObjectPrivate)
{
	d->id = id;
	d->repo = &repo;
	populateHeader();
}

RawObject::RawObject(const RawObject &other)
	: QObject(other.parent())
	, d(other.d)
{
}

RawObject::~RawObject()
{
}



const QByteArray RawObject::data()
{
	if (!isValid()) {
		return QByteArray();
	}

	return id().storage().objectDataFor(d->id);
}

const QString RawObject::extractHeaderForm(const QByteArray &rawData)
{
	int nullByteIndex = rawData.indexOf('\0');
	QString possibleHeader;

	if (nullByteIndex >= 0) {
		possibleHeader = rawData.left(nullByteIndex);
	} else {
		Q_ASSERT(rawData.size() <= 7+9); // should suffice for even absurd headers (up to 9 digits object size)
		possibleHeader = rawData;
	}

	if (isValidHeader(possibleHeader)) {
		return possibleHeader;
	}

	return QString();
}

int RawObject::extractObjectSizeFrom(const QString &header)
{
	if (!header.isNull()) {
		return header.mid(header.indexOf(' ')).toInt();
	}

	return -1;
}

ObjectType RawObject::extractObjectTypeFrom(const QString &header)
{
	return typeFromTypeName(header.left(header.indexOf(' ')));
}

const Id& RawObject::id() const
{
	return d->id;
}

RawObject& RawObject::invalid()
{
	static RawObject invalid;
	return invalid;
}

void RawObject::invalidate()
{
	d->id.invalidate();
}

bool RawObject::isBlob() const
{
	return type() == OBJ_BLOB;
}

bool RawObject::isCommit() const
{
	return type() == OBJ_COMMIT;
}

bool RawObject::isOnlyHeader(const QByteArray &rawData)
{
	return !rawData.isEmpty() && (rawData == extractHeaderForm(rawData).toLatin1());
}

bool RawObject::isTag() const
{
	return type() == OBJ_TAG;
}

bool RawObject::isTree() const
{
	return type() == OBJ_TREE;
}

bool RawObject::isValid() const
{
	return d->id.isValid();
}

bool RawObject::isValidHeader(const QString &possibleHeader)
{
	return possibleHeader.contains(QRegExp("^(blob|commit|tag|tree) \\d+$"));
}

RawObject* RawObject::newInstance(const Id &id)
{
	Q_ASSERT(id.exists());

	ObjectStorage &storage = id.storage();
	ObjectType type = storage.objectTypeFor(id);

	switch(type) {
	case OBJ_BLOB:
		return new Blob(id, id.repo());
	case OBJ_COMMIT:
		return new Commit(id, id.repo());
	case OBJ_TREE:
		return new Tree(id, id.repo());
	default:
		// as long as all types are not yet implemented
		return &RawObject::invalid();
	}
}

RawObject& RawObject::operator=(const RawObject &other)
{
	setParent(other.parent());

	d = other.d;

	return *this;
}

bool RawObject::operator==(const RawObject &other) const
{
	return id() == other.id();
}

void RawObject::populateHeader()
{
	Q_ASSERT(id().isValid());

	if (!isValid()) {
		return;
	}

	ObjectStorage &store = id().storage();

	d->dataSize = store.objectSizeFor(id());
	d->type     = store.objectTypeFor(id());
}

Repo& RawObject::repo() const
{
	return *(d->repo);
}

int RawObject::size() const
{
	return d->dataSize;
}

Blob& RawObject::toBlob() const
{
	return isBlob() ? *(Blob*)this : Blob::invalid();
}

Commit& RawObject::toCommit() const
{
	return isCommit() ? *(Commit*)this : Commit::invalid();
}

Tree& RawObject::toTree() const
{
	return isTree() ? *(Tree*)this : Tree::invalid();
}

ObjectType RawObject::type() const
{
	return d->type;
}

ObjectType RawObject::typeFromTypeName(const QString &name)
{
	static const char* ObjectTypeNames[] = {"", "commit", "tree", "blob", "tag"};

	for (uint i=4/*maximum index in ObjectTypeNames[]*/; i > 0; --i) {
		if (name == ObjectTypeNames[i]) {
			return (ObjectType)i;
		}
	}

	return OBJ_NONE;
}

const QString RawObject::typeNameFromType(const ObjectType type)
{
	static const char* ObjectTypeNames[] = {"", "commit", "tree", "blob", "tag"};
	return ObjectTypeNames[type];
}



#include "RawObject.moc"
