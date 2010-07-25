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

#include "PackedStorageObject.h"

#include "PackedStorage.h"
#include "PackedStorage_p.h"

#include <KDebug>

#include <QFile>

using namespace Git;



#define Sha1Size  20



PackedStorageObject::PackedStorageObject(PackedStorage &storage, quint32 offset)
	: QObject(&storage)
	, d(new PackedStorageObjectPrivate)
{
	d->id = storage.idForObjectAt(offset);
	d->offset = offset;
	d->storage = &storage;
	readHeader();
}

PackedStorageObject::PackedStorageObject(PackedStorage &storage, quint32 offset, const QString &id)
	: QObject(&storage)
	, d(new PackedStorageObjectPrivate)
{
	d->id = id;
	d->offset = offset;
	d->storage = &storage;
	readHeader();
}

PackedStorageObject::PackedStorageObject(const PackedStorageObject &other)
	: QObject(other.d->storage)
	, d(other.d)
{
}



const QByteArray PackedStorageObject::data()
{
	QByteArray unpackedData;

	QFile &pack = d->storage->packFile();
	if (isDeltified()) {
		pack.seek(d->deltaDataOffset);
	} else {
		pack.seek(d->dataOffset);
	}

	// check for zlib header
	Q_ASSERT(pack.peek(2) == QByteArray(QByteArray::fromHex("789c"), 2));

	while (unpackedData.size() < size()) {
		QByteArray packedData = pack.read(4096);
		if (packedData.size() == 0) {
			kError() << "could not read packed data for" << d->id << "in" << d->storage->d->name;
			/** @todo throw exception */
			return QByteArray();
		}
		unpackedData.append(ObjectStorage::inflate(packedData));
	}

	if (unpackedData.size() > size()) {
			kError() << "unpacked more data than expected for" << d->id << "in" << d->storage->d->name;
			/** @todo throw exception */
			return QByteArray();
	}

	Q_ASSERT(unpackedData.size() == size());

	return unpackedData;
}

const QByteArray PackedStorageObject::finalData()
{
	if (isDeltified()) {
		return patchedData();
	} else {
		return data();
	}
}

quint32 PackedStorageObject::finalSize()
{
	if (isDeltified()) {
		return d->patchedSize;
	} else {
		return d->size;
	}
}

ObjectType PackedStorageObject::finalType()
{
	if (isDeltified()) {
		return d->baseObject->finalType();
	} else {
		return d->type;
	}
}

const QString PackedStorageObject::id()
{
	return d->id;
}

bool PackedStorageObject::isDeltified()
{
	switch (type()) {
	case OBJ_OFS_DELTA:
	case OBJ_REF_DELTA:
		return true;
	default:
		return false;
	}
}

quint32 PackedStorageObject::objectSizeIn(const QByteArray &delta, quint32 &pos)
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

const QByteArray PackedStorageObject::patchedData()
{
	kDebug() << "patching base object" << d->baseObject->id() << "with delta" << id() << "in" << d->storage->d->name;
	QByteArray base = d->baseObject->finalData();
	QByteArray delta = data();
	return patchDelta(base, delta);
}

const QByteArray PackedStorageObject::patchDelta(const QByteArray &base, const QByteArray &delta)
{
	Q_ASSERT(delta.size() >= 4); // minimal delta size

	if (d->baseSize != base.size()) {
		kError() << "invalid delta header for" << id() << "in" << d->storage->d->name;
		return QByteArray();
	}

	quint32 pos = d->patchDataOffset;

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
				cpOff + cpSize > d->baseSize ||
				cpSize > d->patchedSize) {
				kError() << "In delta" << id() << "at" << QString::number(cmd_pos, 16).prepend("0x") << "error copying from source data in" << d->storage->d->name;
				break;
			}
			patched += base.mid(cpOff, cpSize);
			kDebug() << "In delta" << id() << "at" << QString::number(cmd_pos, 16).prepend("0x") << "copy" << cpSize << "from source at" << QString::number(cpOff, 16).prepend("0x") << "in" << d->storage->d->name;
		} else if (cmd) {
			if (cmd > d->patchedSize) {
				kError() << "In delta" << id() << "at" << QString::number(cmd_pos, 16).prepend("0x") << "error copying from delta in" << d->storage->d->name;
				break;
			}
			patched += delta.mid(pos, cmd);
			kDebug() << "In delta" << id() << "at" << QString::number(cmd_pos, 16).prepend("0x") << "copy" << cmd << "from delta at" << QString::number(pos, 16).prepend("0x") << "in" << d->storage->d->name;
			pos += cmd;
		} else {
			kError() << "In delta" << id() << "at" << QString::number(cmd_pos, 16).prepend("0x") << "unexpected opcode 0 in" << d->storage->d->name;
			/** @todo raise exception */
			return QByteArray();
		}
	}

	// sanity check
	if (pos != delta.size()) {
		kError() << "patched" << pos << "bytes out of" << delta.size() << "byte delta in" << d->storage->d->name;
		return QByteArray();
	} else if (d->patchedSize != patched.size()) {
		kError() << "patched data has size" << patched.size() << "but should have benn" << d->patchedSize << "!=" << " in" << d->storage->d->name;
		return QByteArray();
	}

	return patched;
}

void PackedStorageObject::readDeltaHeader()
{
	QFile &pack = d->storage->packFile();
	pack.seek(d->dataOffset);

	d->deltaDataOffset = d->dataOffset;

	qint32 baseOffset = 0;
	if (type() == OBJ_OFS_DELTA) {
		quint8 c = pack.read(1)[0]; // read 1 byte
		baseOffset = c & 0x7f;      // get the lower 7 bits
		d->deltaDataOffset += 1;    // we read 1 byte

		while ((c & 0x80) != 0) {  // untill the highest bit is 0
			c = pack.read(1)[0];     // read the next byte and increment index
			baseOffset += 1;         // ???
			baseOffset <<= 7;        // shift by 7 bits to make way for the next piece of the offset
			baseOffset |= c & 0x7f;  // get the lower 7 bits and put them to their proper location
			d->deltaDataOffset += 1; // we read another byte
		}
		baseOffset = d->offset - baseOffset;
	} else {
		baseOffset = d->storage->dataOffsetFor(pack.read(Sha1Size).toHex());
		d->deltaDataOffset += Sha1Size;
	}

	Q_ASSERT(baseOffset != 0);
	d->baseObject = d->storage->packObjectFor(d->storage->idForObjectAt(baseOffset));
	Q_ASSERT(d->baseObject);


	const QByteArray delta = data();
	Q_ASSERT(delta.size() >= 4); // minimal delta size

	d->patchDataOffset = 0;
	d->baseSize = objectSizeIn(delta, d->patchDataOffset/* = 0 */);
	Q_ASSERT(d->patchDataOffset != 0);

	if (d->baseSize != d->baseObject->finalSize()) {
		kError() << "invalid delta header for" << id() << "in" << d->storage->d->name;
	}

	d->patchedSize = objectSizeIn(delta, d->patchDataOffset);
	Q_ASSERT(d->patchDataOffset != 0);
}

void PackedStorageObject::readHeader()
{
	QFile &pack = d->storage->packFile();
	pack.seek(d->offset);

	d->dataOffset = d->offset;

	quint8 c = pack.read(1)[0]; // read 1 byte
	d->size = c & 0xf;          // the lowest 4 bits are the lowest 4 bits of the final object size
	d->type = (ObjectType)((c >> 4) & 7); // the next 3 bits are the object type
	int shift      = 4;         // shift the next piece of size information by 4 bits
	d->dataOffset += 1;         // we have read another byte

	while ((c & 0x80) != 0) { // untill the highest bit is 0
		c = pack.read(1)[0];    // read 1 byte

		d->size |= ((c & 0x7f) << shift); // get the lower 7 bits and put them to their proper location in destSize
		shift         += 7; // shift the next piece of size information by another 7 bits
		d->dataOffset += 1; // we have read another byte
	}

	if (isDeltified()) {
		readDeltaHeader();
	}
}

inline quint32 PackedStorageObject::size()
{
	return d->size;
}

inline ObjectType PackedStorageObject::type()
{
	return d->type;
}



#include "PackedStorageObject.moc"
