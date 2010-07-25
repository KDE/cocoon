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

#ifndef LOOSESTORAGE_H
#define LOOSESTORAGE_H

#include "ObjectStorage.h"

class LooseStorageTest;
class LooseStorageCachingTest;

namespace Git {

class LooseStoragePrivate;



class KDE_EXPORT LooseStorage : public ObjectStorage
{
	Q_OBJECT

	public:
		explicit LooseStorage(Repo &repo);
		LooseStorage(const LooseStorage &other);
		virtual ~LooseStorage();

		const QList<Id> allIds();
		const QByteArray objectDataFor(const Id &id);
		RawObject*       objectFor(const Id &id);
		int              objectSizeFor(const Id &id);
		ObjectType       objectTypeFor(const Id &id);

	protected:
		void loadHeaderDataFor(const Id &id);
		const QByteArray rawDataFor(const Id &id, const qint64 maxRead = -1);
		const QString sourceFor(const Id &id);

	private:
		QSharedDataPointer<LooseStoragePrivate> d;

	friend class ::LooseStorageTest;
	friend class ::LooseStorageCachingTest;
};

}

#endif // LOOSESTORAGE_H
