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

#include "ObjectStorage.h"

#include <QRegExp>

using namespace Git;



RawObject::RawObject(const QString &id, ObjectStorage *storage)
	: QObject((QObject*)storage)
	, m_id(id)
	, m_storage(storage)
{
	populateWith(m_storage->rawDataFor(m_id));
}

RawObject::RawObject(const QString &id, const QByteArray &rawData, ObjectStorage *storage)
	: QObject((QObject*)storage)
	, m_id(id)
	, m_storage(storage)
{
	populateWith(rawData);
}



const QByteArray& RawObject::data() const
{
	return m_data;
}

const QString RawObject::extractHeaderForm(const QByteArray &rawData) const
{
	QString possibleHeader = rawData.left(rawData.indexOf('\0'));
	if (isValidHeader(possibleHeader)) {
		return possibleHeader;
	}

	return QString();
}

int RawObject::extractObjectSizeFrom(const QString &header) const
{
	if (!header.isNull()) {
		return header.mid(header.indexOf(' ')).toInt();
	}

	return -1;
}

const QString RawObject::extractObjectTypeFrom(const QString &header) const
{
	return header.left(header.indexOf(' '));
}

const QString& RawObject::id() const
{
	return m_id;
}

bool RawObject::isValidHeader(const QString &possibleHeader) const
{
	return possibleHeader.contains(QRegExp("^(blob|commit|tag|tree) \\d+$"));
}

void RawObject::populateWith(const QByteArray &rawData)
{
	QString header = extractHeaderForm(rawData);
	if (!isValidHeader(header)) {
		return;
	}

	m_data = rawData.mid(header.size()+1);
	m_size = extractObjectSizeFrom(header);
	m_type = extractObjectTypeFrom(header);
}



#include "RawObject.moc"
