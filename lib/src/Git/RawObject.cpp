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

#include "RawObject.h"

#include "Blob.h"
#include "Commit.h"
#include "ObjectStorage.h"
#include "Repo.h"
#include "Tree.h"

#include <KDebug>

#include <QRegExp>

using namespace Git;



RawObject::RawObject(const QString &id, QObject *parent)
	: QObject(parent)
	, m_data()
	, m_dataSize(-1)
	, m_id(id)
	, m_repo(0)
	, m_type(OBJ_NONE)
{
	kWarning() << "creating object without storage:" << id;
}

RawObject::RawObject(const QString &id, Repo &repo)
	: QObject((QObject*)&repo)
	, m_data()
	, m_dataSize(-1)
	, m_id(id)
	, m_repo(&repo)
	, m_type(OBJ_NONE)
{
	populateHeader();
}



const QByteArray& RawObject::data()
{
	if (m_data.isNull()) {
		m_data = m_repo->storageFor(m_id)->objectDataFor(m_id);
	}

	return m_data;
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

const QString& RawObject::id() const
{
	return m_id;
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

bool RawObject::isValidHeader(const QString &possibleHeader)
{
	return possibleHeader.contains(QRegExp("^(blob|commit|tag|tree) \\d+$"));
}

RawObject* RawObject::newInstance(const QString &id, Repo &repo)
{
	ObjectStorage *storage = repo.storageFor(id);
	QString actualId = storage->actualIdFor(id);
	ObjectType type = storage->objectTypeFor(actualId);

	switch(type) {
	case OBJ_BLOB:
		return new Blob(actualId, repo);
	case OBJ_COMMIT:
		return new Commit(actualId, repo);
	case OBJ_TREE:
		return new Tree(actualId, repo);
	default:
		// as long as all types are not yet implemented
		return new RawObject(actualId, repo);
	}
}

void RawObject::populateHeader()
{
	Q_ASSERT(m_repo);
	ObjectStorage *store = m_repo->storageFor(id());

//	m_data = store->objectDataFor(id());
	m_dataSize = store->objectSizeFor(id());
	m_type = store->objectTypeFor(id());
}

Repo& RawObject::repo() const
{
	return *m_repo;
}

int RawObject::size() const
{
	return m_dataSize;
}

ObjectType RawObject::type() const
{
	return m_type;
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
