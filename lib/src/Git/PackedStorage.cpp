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

#include "RawObject.h"
#include "Repo.h"

#include <KDebug>
#include <KFilterBase>

#include <QDir>

#include <netinet/in.h>

using namespace Git;



#define FanOutCount   256
#define SHA1Size       20
#define IdxOffsetSize   4
#define OffsetSize      4
#define CrcSize         4

#define OffsetStart  FanOutCount * IdxOffsetSize
#define SHA1Start    OffsetStart + OffsetSize
#define EntrySize    OffsetSize + SHA1Size
#define EntrySizeV2  SHA1Size + CrcSize + OffsetSize



const char* ObjectTypeNames[] = {"", "commit", "tree", "blob", "tag"};



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

	int pos = SHA1Start;
	for(uint i=0; i < d->size; ++i) {
		QString id = readIndexFrom(pos, SHA1Size).toHex();
		pos += EntrySize;
		ids << id;
	}

	return ids;
}

const QStringList PackedStorage::allIds_v2()
{
	QStringList ids;

	int pos = OffsetStart;
	for(uint i=0; i < d->size; ++i) {
		QString id = readIndexFrom(pos, SHA1Size).toHex();
		pos += SHA1Size;
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
		QString midId = readIndexFrom(SHA1Start + mid * EntrySize, SHA1Size).toHex();
		int cmp = midId.compare(id);

		if (cmp < 0) {
			first = mid + 1;
		} else if (cmp > 0) {
			last = mid;
		} else {
			int pos = OffsetStart + mid * EntrySize;
			int offset = ntohl(*(uint32_t*)readIndexFrom(pos, OffsetSize).data());
			return offset;
		}
	}

	kDebug() << d->name << "has no offset for" << id;

	return -1;
}

int PackedStorage::dataOffsetFor_v2(const QString &id)
{
	int slot = (quint8)QByteArray::fromHex(id.toLatin1())[0];

	int first = d->indexDataOffsets[slot];
	int last = d->indexDataOffsets[slot+1];
	while (first < last) {
		int mid = (first + last) / 2;
		QString midId = readIndexFrom(OffsetStart + (mid * SHA1Size), SHA1Size).toHex();
		int cmp = midId.compare(id);

		if (cmp < 0) {
			first = mid + 1;
		} else if (cmp > 0) {
			last = mid;
		} else {
			int pos = OffsetStart + (d->size * (SHA1Size + CrcSize)) + (mid * OffsetSize);
			int offset = ntohl(*(uint32_t*)readIndexFrom(pos, OffsetSize).data());
			kDebug() << d->name << "found offset" << QString::number(offset, 16) << "for" << id;
			return offset;
		}
	}

	kDebug() << d->name << "has no offset for" << id;

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
		quint32 pos = ntohl(*(uint32_t*)readIndexFrom(i*IdxOffsetSize, IdxOffsetSize).data());
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
	d->indexGlobalDataOffset = 0;
}

void PackedStorage::initIndexVersion_v2()
{
	d->indexVersion = 2;
	d->indexGlobalDataOffset = 8;
}

void PackedStorage::initPack()
{
	bool ok = d->packFile.open(QFile::ReadOnly);
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

const QByteArray PackedStorage::patchDelta(const QByteArray &base, const QByteArray &delta)
{
	quint32 pos = 0;
	int srcSize = patchDeltaHeaderSize(delta, pos/* = 0 */);
	Q_ASSERT(pos != 0);
	if (srcSize != base.size()) {
		kError() << "invalid delta data";
		return QByteArray();
	}

	/*int destSize =*/ patchDeltaHeaderSize(delta, pos);
	Q_ASSERT(pos != 0);

	QByteArray patched;

	while (pos < delta.size()) {
		unsigned char c = delta[pos];
		pos += 1;
		if ((c & 0x80) != 0) {
			pos -= 1;
			int cpSize = 0;
			quint32 cpOff = 0;

			if ((c & 0x01) != 0) { cpOff   = delta[pos += 1]; }
			if ((c & 0x02) != 0) { cpOff  |= delta[pos += 1] <<  8; }
			if ((c & 0x04) != 0) { cpOff  |= delta[pos += 1] << 16; }
			if ((c & 0x08) != 0) { cpOff  |= delta[pos += 1] << 24; }
			if ((c & 0x10) != 0) { cpSize  = delta[pos += 1]; }
			if ((c & 0x20) != 0) { cpSize |= delta[pos += 1] <<  8; }
			if ((c & 0x40) != 0) { cpSize |= delta[pos += 1] << 16; }
			if (cpSize == 0) { cpSize = 0x10000; }
			pos += 1;
			patched += base.mid(cpOff, cpSize);
		} else if (c != 0) {
			patched += delta.mid(pos, c);
			pos += c;
		} else {
			kError() << d->name << "has invalid delta data";
			/** @todo raise exception */
		}
	}

	return patched;
}

quint32 PackedStorage::patchDeltaHeaderSize(const QByteArray &delta, quint32 &pos)
{
	quint32 size = 0;
	quint32 shift = 0;

	char c;
	do {
		c = delta[pos];

		pos += 1;
		size |= (c & 0x7F) << shift;
		shift += 7;
	} while ((c & 0x80) != 0);

	return size;
}

const QByteArray PackedStorage::rawDataFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->rawData.contains(actualId) || RawObject::isOnlyHeader(d->rawData[actualId])) {
		kDebug() << "loading data" << actualId << "from" << d->name;

		QByteArray data = unpackObjectFrom(actualId);
		data.prepend('\0');
		data.prepend(d->objectTypes[actualId].toLatin1() + " " + QString::number(d->objectSizes[actualId]).toLatin1());

		d->rawData[actualId] = data;
	}

	return d->rawData[actualId];
}

const QByteArray PackedStorage::rawHeaderFor(const QString &id)
{
	kDebug() << "loading header" << id << "from" << d->name;
	return rawDataFor(id);
}

RawObject* PackedStorage::rawObjectFor(const QString &id)
{
	const QString actualId = actualIdFor(id);

	if (!d->rawObjects.contains(actualId)) {
		kDebug() << "loading object" << actualId << "from" << d->name;
		d->rawObjects[actualId] = RawObject::newInstance(actualId, repo());
	}

	return d->rawObjects[actualId];
}

const QByteArray PackedStorage::readIndexFrom(int offset, int length)
{
	Q_ASSERT(d->indexFile.isOpen());

	offset += d->indexGlobalDataOffset;

	Q_ASSERT(length != 0);

	if (length < 0) {
		length = 1;
	}

	d->indexFile.seek(offset);
	return d->indexFile.read(length);
}

const QByteArray PackedStorage::unpackCompressed(int offset, int destSize)
{
	QByteArray unpackedData;
	d->packFile.seek(offset);

	while (unpackedData.size() < destSize) {
		QByteArray packedData = d->packFile.read(4096);
		if (packedData.size() == 0) {
			kError() << "could not read packed data in" << d->name;
			/** @todo throw exception */
			return QByteArray();
		}
		unpackedData.append(inflate(packedData));
	}

	if (unpackedData.size() > destSize) {
			kError() << "unpacked more data than expected in" << d->name;
			/** @todo throw exception */
			return QByteArray();
	}

	Q_ASSERT(unpackedData.size() == destSize);

	return unpackedData;
}

const QByteArray  PackedStorage::unpackDeltified(const QString &id, int type, int offset, int objOffset, int size)
{
	d->packFile.seek(offset);
	QByteArray data = d->packFile.read(SHA1Size);

	int baseOffset = -1;
	if (type == OBJ_OFS_DELTA) {
		int i = 0;
		unsigned char c = data[0];
		baseOffset = c & 0x7f;
		while ((c & 0x80) != 0) {
			c = data[i += 1];
			baseOffset += 1;
			baseOffset <<= 7;
			baseOffset |= c & 0x7f;
		}
		baseOffset = objOffset - baseOffset;
		offset += i + 1;
	} else {
		baseOffset = dataOffsetFor(data/*.toHex() ?*/);
		offset += SHA1Size;
	}

	Q_ASSERT(baseOffset >= 0);

	QByteArray base = unpackObjectFrom(id, baseOffset);
	/** @todo get object type */
	QByteArray delta = unpackCompressed(offset, size);
	return patchDelta(base, delta);
}

const QByteArray PackedStorage::unpackObjectFrom(const QString &id, int offset)
{
	if (offset < 0) {
		offset = dataOffsetFor(id);
		Q_ASSERT(offset >= 0);
	}

	int objectOffset = offset;
	d->packFile.seek(offset);

	char c = d->packFile.read(1)[0];
	int size = c & 0xf;
	int type = (c >> 4) & 7;
	int shift = 4;
	offset += 1;
	while ((c & 0x80) != 0) {
		c = d->packFile.read(1)[0];
		size |= ((c & 0x7f) << shift);
		shift += 7;
		offset += 1;
	}

	QByteArray rawData;
	switch (type) {
	case OBJ_OFS_DELTA:
	case OBJ_REF_DELTA:
		kDebug() << "unpacking delta from" << d->name;
		rawData = unpackDeltified(id, type, offset, objectOffset, size);
		d->objectSizes[id] = rawData.size();
		break;
	case OBJ_BLOB:
	case OBJ_COMMIT:
	case OBJ_TAG:
	case OBJ_TREE:
		rawData = unpackCompressed(offset, size);
		d->objectSizes[id] = size;
		d->objectTypes[id] = ObjectTypeNames[type];
		break;
	default:
		kError() << "invalid type" << type << "in" << d->name;
		/** @todo throw exception */
		return QByteArray();
	}

	return rawData;
}



#include "PackedStorage.moc"
