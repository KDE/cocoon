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

#include "Commit.h"
#include "ObjectStorage.h"

#include <KDebug>

#include <QRegExp>

using namespace Git;



RawObject::RawObject(const QString &id, QObject *parent)
	: QObject(parent)
	, m_id(id)
	, m_storage(0)
{
	kDebug() << "creating object without storage:" << id;
}

RawObject::RawObject(const QString &id, ObjectStorage &storage)
	: QObject((QObject*)&storage)
	, m_id(id)
	, m_storage(&storage)
{
	populateWith(m_storage->rawHeaderFor(m_id));
}

RawObject::RawObject(const QString &id, const QByteArray &rawData, ObjectStorage &storage)
	: QObject((QObject*)&storage)
	, m_id(id)
	, m_storage(&storage)
{
	populateWith(rawData);
}



const QByteArray& RawObject::data()
{
	if (m_data.isNull()) {
		Q_ASSERT(m_storage);
		populateWith(m_storage->rawDataFor(m_id));
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
		Q_ASSERT(rawData.size() <= 7+9); // should suffice for even absurd headers (at least 9 digits size)
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

const QString RawObject::extractObjectTypeFrom(const QString &header)
{
	return header.left(header.indexOf(' '));
}

const QString& RawObject::id() const
{
	return m_id;
}

bool RawObject::isBlob() const
{
	return type() == "blob";
}

bool RawObject::isCommit() const
{
	return type() == "commit";
}

bool RawObject::isOnlyHeader(const QByteArray &rawData)
{
	return !rawData.isEmpty() && (rawData == extractHeaderForm(rawData).toLatin1());
}

bool RawObject::isTag() const
{
	return type() == "tag";
}

bool RawObject::isTree() const
{
	return type() == "tree";
}

bool RawObject::isValidHeader(const QString &possibleHeader)
{
	return possibleHeader.contains(QRegExp("^(blob|commit|tag|tree) \\d+$"));
}

void RawObject::populateWith(const QByteArray &rawData)
{
	QString header = extractHeaderForm(rawData);
	if (!isValidHeader(header)) {
		kDebug() << "invalid header for object" << id();
		return;
	}

	m_data = rawData.mid(header.size()+1);
	m_type = extractObjectTypeFrom(header);
}

RawObject* RawObject::newInstance(const QString &id, ObjectStorage &storage)
{
	if (extractObjectTypeFrom(storage.rawHeaderFor(id)) == "commit") {
		return new Commit(id, storage);
	}

	return new RawObject(id, storage);
}

ObjectStorage* RawObject::storage() const
{
	return m_storage;
}

const QString& RawObject::type() const
{
	return m_type;
}



#include "RawObject.moc"
