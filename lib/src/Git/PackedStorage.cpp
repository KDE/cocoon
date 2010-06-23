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

#include "PackedStorage.h"

#include "PackedStorageObject.h"
#include "RawObject.h"
#include "Repo.h"

#include <KDebug>
#include <KFilterBase>

#include <QDir>

#include <netinet/in.h>

using namespace Git;



#define FanOutCount        256
#define Sha1Size            20
#define IdxOffsetSize        4
#define OffsetSize           4
#define CrcSize              4
#define LargePackOffsetSize  8

#define FanOutEntrySize   IdxOffsetSize
#define FanOutTableSize  (FanOutCount * FanOutEntrySize)
#define Sha1TableSize    (size() * Sha1Size)
#define CrcTableSize     (size() * CrcSize)


#define indexV1_OffsetTableEntrySize  (OffsetSize + Sha1Size)
#define indexV1_OffsetTableSize       (size() * indexV1_OffsetTableEntrySize)

#define indexV1_HeaderSize         0
#define indexV1_FanOutTableStart   indexV1_HeaderSize
#define indexV1_OffsetTableStart  (indexV1_FanOutTableStart + FanOutTableSize)
#define indexV1_TrailerStart      (indexV1_OffsetTableStart + indexV1_OffsetTableSize)


#define indexV2_OffsetTableSize        (size() * OffsetSize)

#define indexV2_HeaderSize         8
#define indexV2_FanOutTableStart   indexV2_HeaderSize
#define indexV2_Sha1TableStart    (indexV2_FanOutTableStart + FanOutTableSize)
#define indexV2_CrcTableStart     (indexV2_Sha1TableStart + Sha1TableSize)
#define indexV2_OffsetTableStart  (indexV2_CrcTableStart + CrcTableSize)
#define indexV2_TrailerStart      (indexV2_OffsetTableStart + indexV2_OffsetTableSize)



PackedStorage::PackedStorage(const QString &name, Repo &repo)
	: ObjectStorage(repo)
	, d(new PackedStoragePrivate)
{
	d->name = name;
	d->indexFile.setFileName(QString("%1/objects/pack/%2.idx").arg(repo.gitDir()).arg(d->name));
	d->packFile.setFileName(QString("%1/objects/pack/%2.pack").arg(repo.gitDir()).arg(d->name));

	initIndex();
	initPack();
}

PackedStorage::PackedStorage(const PackedStorage &other)
	: ObjectStorage(other)
	, d(other.d)
{
}



const QStringList PackedStorage::allIds()
{
	switch(d->indexVersion) {
	case 2:
		return allIds_v2();
	default:
		return allIds_v1();
	}
}

const QStringList PackedStorage::allIds_v1()
{
	QStringList ids;

	int pos = indexV1_OffsetTableStart + OffsetSize;
	for(uint i=0; i < d->size; ++i) {
		QString id = readIndexFrom(pos, Sha1Size).toHex();
		pos += indexV1_OffsetTableEntrySize;
		ids << id;
	}

	return ids;
}

const QStringList PackedStorage::allIds_v2()
{
	QStringList ids;

	int pos = indexV2_Sha1TableStart;
	for(uint i=0; i < d->size; ++i) {
		QString id = readIndexFrom(pos, Sha1Size).toHex();
		pos += Sha1Size;
		ids << id;
	}

	return ids;
}

const QStringList PackedStorage::allNamesIn(const Repo &repo)
{
	QStringList names;

	QDir packsDir = QDir(repo.gitDir() + "/objects/pack");

	foreach (const QString &indexFile, packsDir.entryList(QStringList() << "*.idx")) {
		names << indexFile.left(indexFile.length() - 4); // file name lengh - ".idx"
	}

	return names;
}

int PackedStorage::dataOffsetFor(const QString &id)
{
	switch(d->indexVersion) {
	case 2:
		return dataOffsetFor_v2(id);
	default:
		return dataOffsetFor_v1(id);
	}
}

int PackedStorage::dataOffsetFor_v1(const QString &id)
{
	int slot = (quint8)QByteArray::fromHex(id.toLatin1())[0];

	int first = d->indexDataOffsets[slot];
	int last = d->indexDataOffsets[slot+1];
	while (first < last) {
		int mid = (first + last) / 2;
		QString midId = readIndexFrom(indexV1_OffsetTableStart + OffsetSize + (mid * indexV1_OffsetTableEntrySize), Sha1Size).toHex();
		int cmp = midId.compare(id);

		if (cmp < 0) {
			first = mid + 1;
		} else if (cmp > 0) {
			last = mid;
		} else {
			int pos = indexV1_OffsetTableStart + (mid * indexV1_OffsetTableEntrySize);
			int offset = ntohl(*(uint32_t*)readIndexFrom(pos, OffsetSize).data());
			kDebug() << "found offset" << QString::number(offset, 16).prepend("0x") << "for" << id << "in" << d->name;
			return offset;
		}
	}

	kDebug() << "no offset for" << id << "in" << d->name;

	return -1;
}

int PackedStorage::dataOffsetFor_v2(const QString &id)
{
	int slot = (quint8)QByteArray::fromHex(id.toLatin1())[0];

	int first = d->indexDataOffsets[slot];
	int last = d->indexDataOffsets[slot+1];
	while (first < last) {
		int mid = (first + last) / 2;
		QString midId = readIndexFrom(indexV2_Sha1TableStart + (mid * Sha1Size), Sha1Size).toHex();
		int cmp = midId.compare(id);

		if (cmp < 0) {
			first = mid + 1;
		} else if (cmp > 0) {
			last = mid;
		} else {
			int pos = indexV2_OffsetTableStart + (mid * OffsetSize);
			int offset = ntohl(*(uint32_t*)readIndexFrom(pos, OffsetSize).data());
			kDebug() << "found offset" << QString::number(offset, 16).prepend("0x") << "for" << id << "in" << d->name;
			return offset;
		}
	}

	kDebug() << "no offset for" << id << "in" << d->name;

	return -1;
}

void PackedStorage::initIndex()
{
	bool ok = d->indexFile.open(QFile::ReadOnly);
	Q_ASSERT(ok);
	Q_UNUSED(ok);

	initIndexVersion();

	initIndexOffsets();
}

void PackedStorage::initIndexOffsets()
{
	d->indexDataOffsets << 0;
	for (int i=0; i < FanOutCount; ++i) {
		quint32 fanOutTableStart = d->indexVersion == 2 ? indexV2_FanOutTableStart : indexV1_FanOutTableStart;
		quint32 pos = ntohl(*(uint32_t*)readIndexFrom(fanOutTableStart + (i*FanOutEntrySize), FanOutEntrySize).data());
		if (pos < d->indexDataOffsets[i]) {
			kError() << d->name << "has discontinuous index" << i;
			/** @todo throw exception */
			return;
		}
		d->indexDataOffsets << pos;
	}
	d->size = d->indexDataOffsets.last();
}

void PackedStorage::initIndexVersion()
{
	QByteArray signature = d->indexFile.read(4);
	quint32 version = ntohl(*(uint32_t*)d->indexFile.read(4).data());

	kDebug() << d->name << "index version" << version;

	static const QString packIdxSignature = "\377tOc";
	if (signature == packIdxSignature) {
		if (version != 2) {
			kError() << d->name << "has unknown pack file version" << version;
			/** @todo throw exception */
		}
		initIndexVersion_v2();
	} else {
		initIndexVersion_v1();
	}
}

void PackedStorage::initIndexVersion_v1()
{
	d->indexVersion = 1;
}

void PackedStorage::initIndexVersion_v2()
{
	d->indexVersion = 2;
}

void PackedStorage::initPack()
{
	bool ok = d->packFile.open(QFile::ReadOnly);
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

const QByteArray PackedStorage::objectDataFor(const QString &id)
{
	return packObjectFor(id)->finalData();
}

int PackedStorage::objectSizeFor(const QString &id)
{
	return packObjectFor(id)->finalSize();
}

ObjectType PackedStorage::objectTypeFor(const QString &id)
{
	return packObjectFor(id)->finalType();
}

quint32 PackedStorage::offsetIn(quint32 slot)
{
	quint32 pos = 0;
	switch(d->indexVersion) {
	case 2:
		pos = indexV2_OffsetTableStart + (slot * OffsetSize);
		break;
	default:
		pos = indexV1_OffsetTableStart + (slot * indexV1_OffsetTableEntrySize);
		break;
	}
	return ntohl(*(uint32_t*)readIndexFrom(pos, OffsetSize).data());
}

QFile& PackedStorage::packFile()
{
	Q_ASSERT(d->packFile.isOpen());
	return d->packFile;
}

PackedStorageObject* PackedStorage::packObjectFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->packObjects.contains(actualId)) {
		kDebug() << "loading pack object for" << actualId << "in" << d->name;

		d->packObjects[actualId] = new PackedStorageObject(*this, dataOffsetFor(actualId), actualId);
	}

	return d->packObjects[actualId];
}

RawObject* PackedStorage::objectFor(const QString &id)
{
	const QString actualId = actualIdFor(id);

	if (!d->objects.contains(actualId)) {
		kDebug() << "loading object" << actualId << "in" << d->name;
		d->objects[actualId] = RawObject::newInstance(actualId, repo());
	}

	return d->objects[actualId];
}

const QByteArray PackedStorage::readIndexFrom(int offset, int length)
{
	Q_ASSERT(d->indexFile.isOpen());

	/*Q_ASSERT(length != 0);

	if (length < 0) {
		length = 1;
	}*/
	Q_ASSERT(length > 0);

	d->indexFile.seek(offset);
	return d->indexFile.read(length);
}

int PackedStorage::size()
{
	return d->size;
}



#include "PackedStorage.moc"
