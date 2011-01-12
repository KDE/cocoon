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
#include "LooseStorage_p.h"

#include "Repo.h"

#include <KDebug>

using namespace Git;



#define DEFLATED_HEADER_READ_SIZE  128



LooseStorage::LooseStorage(Repo &repo)
	: ObjectStorage(repo)
	, d(new LooseStoragePrivate(*ObjectStorage::d))
{
	reset();
}

LooseStorage::LooseStorage(const LooseStorage &other)
	: ObjectStorage(other)
	, d(other.d)
{
}

LooseStorage::~LooseStorage()
{
	invalidateIds();
	invalidateObjects();

	// manually delete objects
	foreach (RawObject *obj, d->objects) {
		delete obj;
	}
	d->objects.clear();
}



const QList<Id> LooseStorage::allIds()
{
	if(d->ids.isEmpty()) {
		foreach (const QString &dir, d->objectsDir.entryList()) {
			if (dir.contains(QRegExp("^[0-9a-fA-F]{2}$"))) {
				foreach (const QString &file, QDir(d->objectsDir.path() + "/" + dir).entryList()) {
					if (file.contains(QRegExp("^[0-9a-fA-F]{38}$"))) {
						d->ids << Id(dir + file, *this);
					}
				}
			}
		}
	}

	return d->ids;
}

void LooseStorage::invalidateIds()
{
	foreach (Id id, d->ids) {
		id.invalidate();
	}
}

void LooseStorage::invalidateObjects()
{
	foreach (RawObject *object, d->objects) {
		object->invalidate();
	}
}

void LooseStorage::loadHeaderDataFor(const Id &id)
{
	if (!d->objectSizes.contains(id)) {
		Q_ASSERT(!d->objectTypes.contains(id));
		kDebug() << "Loading header for" << id.toString();

		QString header = RawObject::extractHeaderForm(rawDataFor(id));
		d->objectSizes[id] = RawObject::extractObjectSizeFrom(header);
		d->objectTypes[id] = RawObject::extractObjectTypeFrom(header);
	}
}

const QByteArray LooseStorage::objectDataFor(const Id &id)
{
	if (!id.isValid()) {
		return QByteArray();
	}

	if (!d->objectData.contains(id)) {
		kDebug() << "Loading data for" << id.toString();

		QByteArray rawData = rawDataFor(id);

		d->objectData[id] = rawData.mid(RawObject::extractHeaderForm(rawData).size()+1);
	}

	return d->objectData[id];
}

RawObject& LooseStorage::objectFor(const Id &id)
{
	if (!id.isValid()) {
		return RawObject::invalid();
	}

	if (!d->objects.contains(id)) {
		kDebug() << "load object" << id.toString();
		d->objects[id] = RawObject::newInstance(id, repo());
	}

	return *d->objects[id];
}

int LooseStorage::objectSizeFor(const Id &id)
{
	if (!id.isValid()) {
		return -1;
	}

	loadHeaderDataFor(id);

	return d->objectSizes[id];
}

ObjectType LooseStorage::objectTypeFor(const Id &id)
{
	if (!id.isValid()) {
		return OBJ_NONE;
	}

	loadHeaderDataFor(id);

	return d->objectTypes[id];
}

const QByteArray LooseStorage::rawDataFor(const Id &id, const qint64 maxRead)
{
	Q_ASSERT(id.isValid());

	kDebug() << "Loading raw data for" << id.toString();

	QFile objectFile(sourceFor(id));
	Q_ASSERT(objectFile.exists());

	// open the object's file
	bool ok = objectFile.open(QIODevice::ReadOnly);
	Q_ASSERT(ok);
	Q_UNUSED(ok);

	QByteArray rawData;
	if (maxRead > 0) {
		rawData = objectFile.read(maxRead);
	} else {
		rawData = inflate(objectFile.readAll());
	}

	objectFile.close();

	return rawData;
}

void LooseStorage::reset()
{
	ObjectStorage::reset();

	// manually delete objects
	foreach (RawObject *obj, d->objects) {
		delete obj;
	}
	d->objects.clear();

	Repo *r = d->repo;
	d = new LooseStoragePrivate();
	d->repo = r;
	d->objectsDir = QDir(d->repo->gitDir() + "/objects");
}

const QString LooseStorage::sourceFor(const Id &id)
{
	Q_ASSERT(id.isValid());
	QString idDirPath  = id.toSha1String().left(2);
	QString idFilePath = id.toSha1String().mid(2);

	QString sourcePath = d->objectsDir.filePath("%1/%2").arg(idDirPath).arg(idFilePath);
	return sourcePath;
}



#include "LooseStorage.moc"
