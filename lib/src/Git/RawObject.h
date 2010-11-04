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

#ifndef RAWOBJECT_H
#define RAWOBJECT_H

#include <QObject>

#include "Id.h"

#include <kdemacros.h>

#include <QSharedDataPointer>



class RawObjectTest;
class RawObjectTypeTest;

namespace Git {

class ObjectStorage;
class RawObjectPrivate;
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



class KDE_EXPORT RawObject : public QObject
{
	Q_OBJECT

	protected:
		explicit RawObject(const Id& id, Repo &repo);

	public:
		virtual ~RawObject();

		const QByteArray& data();
		const Id&         id() const;
		int               size() const;
		ObjectType        type() const;

		bool isBlob() const;
		bool isCommit() const;
		bool isTag() const;
		bool isTree() const;

	// static
		static const QString extractHeaderForm(const QByteArray &rawData);
		static int extractObjectSizeFrom(const QString &header);
		static ObjectType extractObjectTypeFrom(const QString &header);
		static bool isOnlyHeader(const QByteArray &rawData);
		static bool isValidHeader(const QString &possibleHeader);
		static RawObject* newInstance(const Id& id, Repo &repo);
		static ObjectType typeFromTypeName(const QString& name);
		static const QString typeNameFromType(const ObjectType type);

	protected:
		virtual void populateHeader();
		Repo& repo() const;

	protected:
		QExplicitlySharedDataPointer<RawObjectPrivate> d;

	friend class ::RawObjectInstantiationTest;
	friend class ::RawObjectTest;
	friend class ::RawObjectTypeTest;
};

}

#endif // RAWOBJECT_H
