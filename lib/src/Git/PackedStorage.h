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

#ifndef PACKEDSTORAGE_H
#define PACKEDSTORAGE_H

#include "ObjectStorage.h"

class QFile;

class PackedStorageTest;
class PackedStorageCachingTest;
class PackedStorageDeltifiedExtractionTest;
class PackedStorageNormalExtractionTest;

namespace Git {

class PackedStorageObject;
class PackedStoragePrivate;



class KDE_EXPORT PackedStorage : public ObjectStorage
{
	Q_OBJECT

	public:
		explicit PackedStorage(const QString &name, Repo &repo);
		PackedStorage(const PackedStorage &other);
		virtual ~PackedStorage();

		const QList<Id>  allIds();
		const QByteArray objectDataFor(const Id &id);
		RawObject&       objectFor(const Id &id);
		int              objectSizeFor(const Id &id);
		ObjectType       objectTypeFor(const Id &id);
		int              size();

	// static
		static const QStringList allNamesIn(const Repo &repo);

	public slots:
		void reset();

	protected:
		void invalidateIds();
		void invalidateObjects();

	private:
		const QList<Id> allIds_v1();
		const QList<Id> allIds_v2();
		const Id idForObjectAt(quint32 offset);
		const Id idIn(quint32 slot);
		void initIndex();
		void initIndexOffsets();
		void initIndexVersion();
		void initIndexVersion_v1();
		void initIndexVersion_v2();
		quint32 dataOffsetFor(const Id &id);
		void initPack();
		quint32 offsetIn(quint32 slot);
		QFile& packFile();
		PackedStorageObject* packObjectFor(const Id &id);
		const QByteArray readIndexFrom(int offset, int length = -1);

	private:
		QExplicitlySharedDataPointer<PackedStoragePrivate> d;

	friend class PackedStorageObject;

	friend class ::PackedStorageTest;
	friend class ::PackedStorageCachingTest;
	friend class ::PackedStorageDeltifiedExtractionTest;
	friend class ::PackedStorageNormalExtractionTest;
};

}

#endif // PACKEDSTORAGE_H
