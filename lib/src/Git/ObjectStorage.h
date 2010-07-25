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

#ifndef OBJECTSTORAGE_H
#define OBJECTSTORAGE_H

#include <QObject>

#include "Id.h"
#include "RawObject.h"

#include <kdemacros.h>

#include <QSharedDataPointer>

namespace Git {

class ObjectStoragePrivate;
class RawObject;



class KDE_EXPORT ObjectStorage : public QObject
{
	Q_OBJECT

	public:
		explicit ObjectStorage(Repo &repo);
		ObjectStorage(const ObjectStorage &other);
		~ObjectStorage();

		Repo& repo() const;

		const QString actualIdFor(const QString &shortId);
		virtual const QList<Id> allIds() = 0;
		virtual QList<RawObject*> allObjects();
		virtual QList<RawObject*> allObjectsByType(const ObjectType type);
		virtual bool contains(const Id &id);
		virtual bool contains(const QString &id);
		virtual const QByteArray objectDataFor(const Id &id) = 0;
		virtual RawObject*       objectFor(const Id &id) = 0;
		virtual int              objectSizeFor(const Id &id) = 0;
		virtual ObjectType       objectTypeFor(const Id &id) = 0;

	protected:
	// static
		static const QByteArray inflate(const QByteArray deflated);

	private:
		QSharedDataPointer<ObjectStoragePrivate> d;
};

}

#endif // OBJECTSTORAGE_H
