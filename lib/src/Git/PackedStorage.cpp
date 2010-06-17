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
	QString actualId = actualIdFor(id);

	if (!d->objectData.contains(actualId)) {
		kDebug() << "loading data for" << actualId << "in" << d->name;

		d->objectData[actualId] = unpackObjectFrom(actualId);
	}

	return d->objectData[actualId];
}

int PackedStorage::objectSizeFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->objectSizes.contains(actualId)) {
		kDebug() << "loading size for" << actualId << "in" << d->name;

		unpackObjectFrom(actualId);
	}

	return d->objectSizes[actualId];
}

quint32 PackedStorage::objectSizeIn(const QByteArray &delta, quint32 &pos)
{
	quint32 size = 0;
	int shift = 0;

	quint8 cmd;
	do {
		cmd = delta[pos++];

		size |= (cmd & 0x7F) << shift; // get the lower 7 bits and put them to their proper location
		shift += 7;                    // shift the next piece of size information by another 7 bits
	} while (cmd & 0x80 && pos < delta.size()); // untill the highest bit is 0

	return size;
}

ObjectType PackedStorage::objectTypeFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->objectTypes.contains(actualId)) {
		kDebug() << "loading type for" << actualId << "in" << d->name;

		unpackObjectFrom(actualId);
	}

	return d->objectTypes[actualId];
}

const QByteArray PackedStorage::patchDelta(const QByteArray &base, const QByteArray &delta)
{
	Q_ASSERT(delta.size() >= 4); // minimal delta size

	quint32 pos = 0;
	quint32 srcSize = objectSizeIn(delta, pos/* = 0 */);
	Q_ASSERT(pos != 0);
	if (srcSize != base.size()) {
		kError() << "invalid delta header in" << d->name;
		return QByteArray();
	}

	quint32 destSize = objectSizeIn(delta, pos);
	Q_ASSERT(pos != 0);

	QByteArray patched;

	while (pos < delta.size()) {
		quint32 cmd_pos = pos;
		quint8 cmd = delta[pos++];
		if (cmd & 0x80) { // highest bit is 1
			quint32 cpOff  = 0;
			quint32 cpSize = 0;

			if (cmd & 0x01) { cpOff   =  (quint8)delta[pos++]; }
			if (cmd & 0x02) { cpOff  |= ((quint8)delta[pos++]) <<  8; }
			if (cmd & 0x04) { cpOff  |= ((quint8)delta[pos++]) << 16; }
			if (cmd & 0x08) { cpOff  |= ((quint8)delta[pos++]) << 24; }
			if (cmd & 0x10) { cpSize  =  (quint8)delta[pos++]; }
			if (cmd & 0x20) { cpSize |= ((quint8)delta[pos++]) <<  8; }
			if (cmd & 0x40) { cpSize |= ((quint8)delta[pos++]) << 16; }
			if (cpSize == 0) { cpSize = 0x10000; }
			if (cpOff + cpSize < cpSize ||
				cpOff + cpSize > srcSize ||
				cpSize > destSize) {
				kError() << "At"<< QString::number(cmd_pos, 16).prepend("0x") << "in delta error copying from source data in" << d->name;
				break;
			}
			patched += base.mid(cpOff, cpSize);
			kDebug() << "At"<< QString::number(cmd_pos, 16).prepend("0x") << "in delta copy" << cpSize << "from source at" << QString::number(cpOff, 16).prepend("0x") << "in" << d->name;
		} else if (cmd) {
			if (cmd > destSize) {
				kError() << "At"<< QString::number(cmd_pos, 16).prepend("0x") << "in delta error copying from delta in" << d->name;
				break;
			}
			patched += delta.mid(pos, cmd);
			kDebug() << "At"<< QString::number(cmd_pos, 16).prepend("0x") << "in delta copy" << cmd << "from delta at" << QString::number(pos, 16).prepend("0x") << "in" << d->name;
			pos += cmd;
		} else {
			kError() << "unexpected delta opcode 0 in" << d->name;
			/** @todo raise exception */
			return QByteArray();
		}
	}

	// sanity check
	if (pos != delta.size()) {
		kError() << "patched" << pos << "bytes out of" << delta.size() << "byte delta in" << d->name;
		return QByteArray();
	} else if (destSize != patched.size()) {
		kError() << "patched data has size" << patched.size() << "but should have benn" << destSize << "!=" << " in" << d->name;
		return QByteArray();
	}

	return patched;
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

const QByteArray PackedStorage::unpackCompressed(int offset, int destSize)
{
	QByteArray unpackedData;
	d->packFile.seek(offset);

	// check for zlib header
	Q_ASSERT(d->packFile.peek(2) == QByteArray(QByteArray::fromHex("789c"), 2));

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

const QByteArray  PackedStorage::unpackDeltified(const QString &id, ObjectType deltaType, quint32 dataOffset, quint32 packEntryOffset, int size)
{
	d->packFile.seek(dataOffset);
	QByteArray data = d->packFile.read(Sha1Size);

	int baseOffset = -1;
	if (deltaType == OBJ_OFS_DELTA) {
		int i = 0;             // index for reading the ith byte
		quint8 c = data[i++];  // read 1 byte and increment index
		baseOffset = c & 0x7f; // get the lower 7 bits

		while ((c & 0x80) != 0) { // untill the highest bit is 0
			c = data[i++];          // read the next byte and increment index
			baseOffset += 1;        // ???
			baseOffset <<= 7;       // shift by 7 bits to make way for the next piece of the offset
			baseOffset |= c & 0x7f; // get the lower 7 bits and put them to their proper location
		}
		baseOffset = packEntryOffset - baseOffset;
		dataOffset += i; // we read a few bytes
	} else {
		baseOffset = dataOffsetFor(data.toHex());
		dataOffset += Sha1Size;
	}

	Q_ASSERT(baseOffset >= 0);

	kDebug() << "patching base object from offset" << QString::number(baseOffset, 16).prepend("0x") << "with delta from offest" << QString::number(packEntryOffset, 16).prepend("0x") << "in" << d->name;
	QByteArray base = unpackObjectFrom(id, baseOffset);
	QByteArray delta = unpackCompressed(dataOffset, size);
	return patchDelta(base, delta);
}

const QByteArray PackedStorage::unpackObjectFrom(const QString &id, qint32 offset)
{
	if (offset < 0) {
		offset = dataOffsetFor(id);
		Q_ASSERT(offset >= 0);
	}

	quint32 packEntryOffset = offset;
	d->packFile.seek(offset);

	quint8 c = d->packFile.read(1)[0]; // read 1 byte
	quint32 destSize = c & 0xf; // the lowest 4 bits are the lowest 4 bits of the final object size
	ObjectType type = (ObjectType)((c >> 4) & 7); // the next 3 bits are the object type
	int shift  = 4;             // shift the next piece of size information by 4 bits
	offset    += 1;             // we have read another byte

	while ((c & 0x80) != 0) { // untill the highest bit is 0
		c = d->packFile.read(1)[0]; // read 1 byte

		destSize |= ((c & 0x7f) << shift); // get the lower 7 bits and put them to their proper location in destSize
		shift    += 7; // shift the next piece of size information by another 7 bits
		offset   += 1; // we have read another byte
	}

	QByteArray rawData;
	switch (type) {
	case OBJ_OFS_DELTA:
	case OBJ_REF_DELTA:
		kDebug() << "unpacking delta in" << d->name;
		rawData = unpackDeltified(id, type, offset, packEntryOffset, destSize);
		d->objectSizes[id] = rawData.size();
		break;
	case OBJ_BLOB:
	case OBJ_COMMIT:
	case OBJ_TAG:
	case OBJ_TREE:
		rawData = unpackCompressed(offset, destSize);
		d->objectSizes[id] = destSize;
		d->objectTypes[id] = type;
		break;
	default:
		kError() << "invalid type" << RawObject::typeNameFromType(type) << "in" << d->name;
		/** @todo throw exception */
		return QByteArray();
	}

	return rawData;
}



#include "PackedStorage.moc"
