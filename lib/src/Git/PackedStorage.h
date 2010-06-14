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

#include "PackedStorage_p.h"

class PackedStorageTest;

namespace Git {



class KDE_EXPORT PackedStorage : public ObjectStorage
{
	Q_OBJECT

	public:
		explicit PackedStorage(const QString &name, Repo &repo);
		PackedStorage(const PackedStorage &other);

		const QStringList allIds();
		const QByteArray objectDataFor(const QString &id);
		RawObject*       objectFor(const QString &id);
		int              objectSizeFor(const QString &id);
		ObjectType       objectTypeFor(const QString &id);
		int              size();

	// static
		static const QStringList allNamesIn(const Repo &repo);

	private:
		const QStringList allIds_v1();
		const QStringList allIds_v2();
		void initIndex();
		void initIndexOffsets();
		void initIndexVersion();
		void initIndexVersion_v1();
		void initIndexVersion_v2();
		int dataOffsetFor(const QString &id);
		int dataOffsetFor_v1(const QString &id);
		int dataOffsetFor_v2(const QString &id);
		void initPack();
		const QByteArray patchDelta(const QByteArray &base, const QByteArray &delta);
		quint32 patchDeltaHeaderSize(const QByteArray &delta, quint32 &pos);
		const QByteArray readIndexFrom(int offset, int length = -1);
		const QByteArray unpackCompressed(int offset, int destSize);
		const QByteArray unpackDeltified(const QString &id, int type, int offset, int objOffset, int size);
		const QByteArray unpackObjectFrom(const QString &id, int offset = -1);

	private:
		QSharedDataPointer<PackedStoragePrivate> d;

	friend class ::PackedStorageTest;
};

}

#endif // PACKEDSTORAGE_H
