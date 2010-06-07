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
		, objectsDir()
		, rawData()
		, rawObjects()
	{}
	LooseStoragePrivate(const LooseStoragePrivate &other)
		: QSharedData(other)
		, objectsDir(other.objectsDir)
		, rawData(other.rawData)
		, rawObjects(other.rawObjects)
	{}

	QDir objectsDir;
	QHash<QString, QByteArray> rawData;
	QHash<QString, RawObject*> rawObjects;
};

};

#endif // LOOSESTORAGE_P_H
