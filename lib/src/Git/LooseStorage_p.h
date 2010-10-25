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

#ifndef LOOSESTORAGE_P_H
#define LOOSESTORAGE_P_H

#include "RawObject.h"

#include <QDir>
#include <QHash>

namespace Git {


class LooseStoragePrivate : public QSharedData {
public:
	LooseStoragePrivate()
		: QSharedData()
		, ids()
		, objectData()
		, objects()
		, objectsDir()
		, objectSizes()
		, objectTypes()
	{}
	LooseStoragePrivate(const LooseStoragePrivate &other)
		: QSharedData(other)
		, ids(other.ids)
		, objectData(other.objectData)
		, objects(other.objects)
		, objectsDir(other.objectsDir)
		, objectSizes(other.objectSizes)
		, objectTypes(other.objectTypes)
	{}

	QList<Id> ids;
	QHash<Id, QByteArray> objectData;
	QHash<Id, RawObject*> objects;
	QDir objectsDir;
	QHash<Id, int>        objectSizes;
	QHash<Id, ObjectType> objectTypes;
};

};

#endif // LOOSESTORAGE_P_H
