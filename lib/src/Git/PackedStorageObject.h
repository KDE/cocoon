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

#ifndef PACKEDSTORAGEOBJECT_H
#define PACKEDSTORAGEOBJECT_H

#include "PackedStorage.h"

class PackedStorageTest;
class PackedStorageDeltifiedExtractionTest;

namespace Git {

class PackedStorageObjectPrivate;



class PackedStorageObject : public QObject
{
	Q_OBJECT

	public:
		explicit PackedStorageObject(PackedStorage &storage, quint32 offset);
		explicit PackedStorageObject(PackedStorage &storage, quint32 offset, const Id &id);
		PackedStorageObject(const PackedStorageObject &other);
		~PackedStorageObject();

//		quint32 crc();
		const QByteArray data();
		const QByteArray finalData();
		quint32 finalSize();
		ObjectType finalType();
		const Id& id();
		bool isDeltified();
		quint32 offset();
//		const QString sha1();
		quint32 size();
		ObjectType type();

	private:
		void readHeader();

		// for deltified objects
		quint32 objectSizeIn(const QByteArray &delta, quint32 &pos);
		const QByteArray patchDelta(const QByteArray &base, const QByteArray &delta);
		const QByteArray patchedData();
		void readDeltaHeader();

	private:
		QSharedDataPointer<PackedStorageObjectPrivate> d;
};

}

#endif // PACKEDSTORAGEOBJECT_H
