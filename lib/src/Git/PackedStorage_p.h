/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2010  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#ifndef PACKEDSTORAGE_P_H
#define PACKEDSTORAGE_P_H

#include <QFile>
#include <QHash>

namespace Git {

class PackedStorageObject;
class RawObject;


class PackedStoragePrivate : public QSharedData {
public:
	PackedStoragePrivate()
		: QSharedData()
		, idAt()
		, indexDataOffsets()
		, indexFile()
		, indexVersion(0)
		, name()
		, objectData()
		, objects()
		, offsetFor()
		, packFile()
		, packObjects()
		, size(0)
	{}
	PackedStoragePrivate(const PackedStoragePrivate &other)
		: QSharedData(other)
		, idAt(other.idAt)
		, indexDataOffsets(other.indexDataOffsets)
		, indexFile(other.indexFile.fileName())
		, indexVersion(other.indexVersion)
		, name(other.name)
		, objectData(other.objectData)
		, objects(other.objects)
		, offsetFor(other.offsetFor)
		, packFile(other.packFile.fileName())
		, packObjects(other.packObjects)
		, size(other.size)
	{}

	QHash<quint32, Id> idAt;
	QList<quint32> indexDataOffsets;
	QFile indexFile;
	quint32 indexVersion;
	QString name;
	QHash<Id, QByteArray> objectData;
	QHash<Id, RawObject*> objects;
	QHash<Id, quint32> offsetFor;
	QFile packFile;
	QHash<Id, PackedStorageObject*> packObjects;
	quint32 size;
};

}

#endif // PACKEDSTORAGE_P_H
