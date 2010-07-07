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

#ifndef RAWOBJECT_P_H
#define RAWOBJECT_P_H

#include <QSharedData>

#include "Id.h"



namespace Git {

class Id;
class Repo;

typedef enum {
	OBJ_NONE = 0,
	OBJ_COMMIT = 1,
	OBJ_TREE = 2,
	OBJ_BLOB = 3,
	OBJ_TAG = 4,
//	OBJ_SOMETHING = 5, // reserved
	OBJ_OFS_DELTA = 6,
	OBJ_REF_DELTA = 7,
} ObjectType;



class RawObjectPrivate : public QSharedData {
public:
	RawObjectPrivate()
		: QSharedData()
		, data()
		, dataSize(-1)
		, id()
		, repo(0)
		, type(OBJ_NONE)
	{}
	RawObjectPrivate(const RawObjectPrivate &other)
		: QSharedData(other)
		, data(other.data)
		, dataSize(other.dataSize)
		, id(other.id)
		, repo(other.repo)
		, type(other.type)
	{}

	QByteArray  data;
	int         dataSize;
	Id          id;
	Repo       *repo;
	ObjectType  type;
};

}

#endif // RAWOBJECT_P_H
