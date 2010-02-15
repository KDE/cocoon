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

#include "LooseStorage.h"

#include "Repo.h"

#include <KDebug>
#include <KFilterBase>

using namespace Git;



LooseStorage::LooseStorage(Repo &repo)
	: ObjectStorage(repo)
	, d(new LooseStoragePrivate)
{
	d->objectsDir = QDir(repo.gitDir());
	d->objectsDir.cd("objects");
}

LooseStorage::LooseStorage(const LooseStorage &other)
	: ObjectStorage(other)
	, d(other.d)
{
}



QList<RawObject*> LooseStorage::allObjects()
{
	QList<RawObject*> objects;

	foreach (const QString &dir, d->objectsDir.entryList()) {
		if (dir.contains(QRegExp("^[0-9a-fA-F]{2}$"))) {
			foreach (const QString &file, QDir(d->objectsDir.path() + "/" + dir).entryList()) {
				if (file.contains(QRegExp("^[0-9a-fA-F]{38}$"))) {
					kDebug() << "found object:" << dir + file;
					objects << rawObjectFor(dir + file);
				}
			}
		}
	}

	return objects;
}

const QByteArray LooseStorage::inflate(QByteArray deflatedData)
{
	QByteArray inflatedData;

	// get gzip filter for inflation and set it to work on the object's file
	KFilterBase *filter = KFilterBase::findFilterByMimeType("application/x-gzip");
	Q_ASSERT(filter);

	filter->init(QIODevice::ReadOnly);

	// prepare for unpacking
	KFilterBase::Result result = KFilterBase::Ok;
	QByteArray outBuffer; // buffers the uncompressed result
	const int bufferSize = 8*1024;

	// reserve memory
	outBuffer.resize(bufferSize);

	// tell the filter about the data
	filter->setInBuffer(deflatedData.data(), deflatedData.size());

	while (result != KFilterBase::End || result != KFilterBase::Error) {
		// tell the filter about the out buffer
		filter->setOutBuffer(outBuffer.data(), outBuffer.size());

		// no need for reading header

		// do the deed
		result = filter->uncompress();

		if (result == KFilterBase::Error) {
			kWarning() << "Error when uncompressing object";
			break; // Error
		}

		int uncompressedBytes = outBuffer.size() - filter->outBufferAvailable();
		kDebug() << "Uncompressed" << uncompressedBytes << "bytes";

		// append the uncompressed data to the objects data
		inflatedData.append(outBuffer.data(), uncompressedBytes);

		if (result == KFilterBase::End) {
			kDebug() << "Finished unpacking";
			break; // Finished.
		}
	}

	filter->terminate();
	delete filter;

	return inflatedData;
}

const QByteArray LooseStorage::rawDataFor(const QString &id)
{
	if (!d->rawData.contains(id) || RawObject::isOnlyHeader(d->rawData[id])) {
		kDebug() << "Loading data for" << id;

		QFile objectFile(sourceFor(id));
		Q_ASSERT(objectFile.exists());

		// open the object's file
		bool ok = objectFile.open(QIODevice::ReadOnly);
		Q_ASSERT(ok);

		d->rawData[id] = inflate(objectFile.readAll());
	}

	return d->rawData[id];
}

const QByteArray LooseStorage::rawHeaderFor(const QString &id)
{
	if (!d->rawData.contains(id)) {
		kDebug() << "Loading header for" << id;

		QFile objectFile(sourceFor(id));
		Q_ASSERT(objectFile.exists());

		// open the object's file
		bool ok = objectFile.open(QIODevice::ReadOnly);
		Q_ASSERT(ok);

		d->rawData[id] = RawObject::extractHeaderForm(inflate(objectFile.read(64))).toLatin1();
	}

	return d->rawData[id];
}

RawObject* LooseStorage::rawObjectFor(const QString &id)
{
	if (!d->rawObjects.contains(id)) {
		kDebug() << "load object:" << id;
		d->rawObjects[id] = RawObject::newInstance(id, *this);
	}

	return d->rawObjects[id];
}

const QString LooseStorage::sourceFor(const QString &id)
{
	Q_ASSERT(id.size() >= 7);
	QString idDirPath = id.left(2);
	QString idFilePath = id.mid(2);

	if (id.size() < 40) {
		QStringList possibleFileIds = QDir(d->objectsDir.filePath(idDirPath)).entryList(QStringList() << QString("%1*").arg(idFilePath));
		Q_ASSERT(possibleFileIds.size() == 1);
		idFilePath = possibleFileIds.first();
	}

	QString sourcePath = d->objectsDir.filePath("%1/%2").arg(idDirPath).arg(idFilePath);
	return sourcePath;
}



#include "LooseStorage.moc"
