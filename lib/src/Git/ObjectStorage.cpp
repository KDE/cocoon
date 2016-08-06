/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#include "ObjectStorage.h"
#include "ObjectStorage_p.h"

#include "RawObject.h"

#include <KDebug>
#include <KFilterDev>
#include <KFilterBase>

#include <QStringList>

using namespace Git;



ObjectStorage::ObjectStorage(Repo &repo)
	: QObject((QObject*)&repo)
	, d(new ObjectStoragePrivate)
{
	d->repo = &repo;
}

ObjectStorage::ObjectStorage(const ObjectStorage &other)
	: QObject(other.parent())
	, d(other.d)
{
}

ObjectStorage::~ObjectStorage()
{
}





const QString ObjectStorage::actualIdFor(const QString &shortId)
{
	if(shortId.size() < 40) {
		/** @todo make the search more intelligent. allIds() should return a sorted list. */
		foreach(const Id &idFromList, allIds()) {
			if (idFromList.toSha1String().startsWith(shortId)) {
				return idFromList.toSha1String();
			}
		}

		return QString();
	}

	Q_ASSERT(shortId.size() == 40);

	return shortId;
}

QList<RawObject> ObjectStorage::allObjects()
{
	QList<RawObject> objects;

	foreach (const Id &id, allIds()) {
		objects << objectFor(id);
	}

	return objects;
}

QList<RawObject> ObjectStorage::allObjectsByType(const ObjectType type)
{
	QList<RawObject> objects;

	foreach (const RawObject &object, allObjects()) {
		if (object.type() == type) {
			objects << object;
		}
	}

	return objects;
}

bool ObjectStorage::contains(const Id &id)
{
	return id.isValid() ? allIds().contains(id) : false;
}

bool ObjectStorage::contains(const QString &id)
{
	if (id.isEmpty())  {
		return false;
	}

	QString actualId = actualIdFor(id);

	/** @todo make the search more intelligent. allIds() should return a sorted list. */
	foreach (const Id &idInList, allIds()) {
		if (idInList == actualId) {
			return true;
		}
	}

	return false;
}

const QByteArray ObjectStorage::inflate(QByteArray deflatedData)
{
	QByteArray inflatedData;

	// get gzip filter for inflation and set it to work on the object's file
    KFilterBase *filter = KFilterDev::filterForCompressionType(KFilterDev::GZip);
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
		//kDebug() << "Uncompressed" << uncompressedBytes << "bytes";

		// append the uncompressed data to the objects data
		inflatedData.append(outBuffer.data(), uncompressedBytes);

		if (result == KFilterBase::End) {
			//kDebug() << "Finished unpacking";
			break; // Finished.
		}
	}

	filter->terminate();
	delete filter;

	return inflatedData;
}

Repo& ObjectStorage::repo() const
{
	return *d->repo;
}

void ObjectStorage::reset()
{
	invalidateIds();
	invalidateObjects();
}



#include "ObjectStorage.moc"
