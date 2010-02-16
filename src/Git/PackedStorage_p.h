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
		, indexDataOffset(-1)
		, indexFile()
		, indexDataOffsets()
		, indexVersion(0)
		, name()
		, packFile()
		, rawData()
		, rawObjects()
		, size(0)
	{}
	PackedStoragePrivate(const PackedStoragePrivate &other)
		: QSharedData(other)
		, indexDataOffset(other.indexDataOffset)
		, indexFile(other.indexFile.fileName())
		, indexDataOffsets(other.indexDataOffsets)
		, indexVersion(other.indexVersion)
		, name(other.name)
		, packFile(other.packFile.fileName())
		, rawData(other.rawData)
		, rawObjects(other.rawObjects)
		, size(other.size)
	{}

	int indexDataOffset;
	QFile indexFile;
	QList<quint32> indexDataOffsets;
	quint32 indexVersion;
	QString name;
	QFile packFile;
	QHash<QString, QByteArray> rawData;
	QHash<QString, RawObject*> rawObjects;
	quint32 size;
};

};

#endif // PACKEDSTORAGE_P_H
