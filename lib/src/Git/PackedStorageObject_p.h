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

#ifndef PACKEDSTORAGEOBJECT_P_H
#define PACKEDSTORAGEOBJECT_P_H

#include "RawObject.h"



namespace Git {



class PackedStorageObjectPrivate : public QSharedData {
public:
	PackedStorageObjectPrivate()
		: QSharedData()
		, baseObject(0)
		, baseSize(-1)
		, dataOffset(0)
		, deltaDataOffset(0)
		, id()
		, offset(0)
		, patchDataOffset(0)
		, patchedSize(-1)
		, size(-1)
		, storage(0)
		, type(OBJ_NONE)
	{}
	PackedStorageObjectPrivate(const PackedStorageObjectPrivate &other)
		: QSharedData(other)
		, baseObject(other.baseObject)
		, baseSize(other.baseSize)
		, dataOffset(other.dataOffset)
		, deltaDataOffset(other.deltaDataOffset)
		, id(other.id)
		, offset(other.offset)
		, patchDataOffset(other.patchDataOffset)
		, patchedSize(other.patchedSize)
		, size(other.size)
		, storage(other.storage)
		, type(other.type)
	{}

	PackedStorageObject *baseObject;
	qint32 baseSize;
	quint32 dataOffset;
	qint32 deltaDataOffset;
	Id id;
	quint32 offset;
	quint32 patchDataOffset;
	qint32 patchedSize;
	qint32 size;
	PackedStorage *storage;
	ObjectType type;
};

};

#endif // PACKEDSTORAGEOBJECT_P_H
