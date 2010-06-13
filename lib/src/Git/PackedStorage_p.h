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

class RawObject;


class PackedStoragePrivate : public QSharedData {
public:
	PackedStoragePrivate()
		: QSharedData()
		, indexDataOffsets()
		, indexFile()
		, indexGlobalDataOffset(-1)
		, indexVersion(0)
		, name()
		, objectData()
		, objects()
		, objectSizes()
		, objectTypes()
		, packFile()
		, size(0)
	{}
	PackedStoragePrivate(const PackedStoragePrivate &other)
		: QSharedData(other)
		, indexDataOffsets(other.indexDataOffsets)
		, indexFile(other.indexFile.fileName())
		, indexGlobalDataOffset(other.indexGlobalDataOffset)
		, indexVersion(other.indexVersion)
		, name(other.name)
		, objectData(other.objectData)
		, objects(other.objects)
		, objectSizes(other.objectSizes)
		, objectTypes(other.objectTypes)
		, packFile(other.packFile.fileName())
		, size(other.size)
	{}

	QList<quint32> indexDataOffsets;
	QFile indexFile;
	int indexGlobalDataOffset;
	quint32 indexVersion;
	QString name;
	QHash<QString, QByteArray> objectData;
	QHash<QString, RawObject*> objects;
	QHash<QString, int>        objectSizes;
	QHash<QString, ObjectType> objectTypes;
	QFile packFile;
	quint32 size;
};

};

#endif // PACKEDSTORAGE_P_H
