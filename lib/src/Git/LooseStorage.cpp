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

using namespace Git;



#define DEFLATED_HEADER_READ_SIZE  128



LooseStorage::LooseStorage(Repo &repo)
	: ObjectStorage(repo)
	, d(new LooseStoragePrivate)
{
	d->objectsDir = QDir(repo.gitDir() + "/objects");
}

LooseStorage::LooseStorage(const LooseStorage &other)
	: ObjectStorage(other)
	, d(other.d)
{
}

LooseStorage::~LooseStorage()
{
	if (!d->objects.isEmpty()) {
		foreach (RawObject *object, d->objects) {
			delete object;
		}
	}
	d->objects.clear();
}



const QStringList LooseStorage::allIds()
{
	QStringList ids;

	foreach (const QString &dir, d->objectsDir.entryList()) {
		if (dir.contains(QRegExp("^[0-9a-fA-F]{2}$"))) {
			foreach (const QString &file, QDir(d->objectsDir.path() + "/" + dir).entryList()) {
				if (file.contains(QRegExp("^[0-9a-fA-F]{38}$"))) {
					ids << dir + file;
				}
			}
		}
	}

	return ids;
}

void LooseStorage::loadHeaderDataFor(const QString &id)
{
	if (!d->objectSizes.contains(id)) {
		Q_ASSERT(!d->objectTypes.contains(id));
		kDebug() << "Loading header for" << id;

		QString header = RawObject::extractHeaderForm(rawDataFor(id));
		d->objectSizes[id] = RawObject::extractObjectSizeFrom(header);
		d->objectTypes[id] = RawObject::extractObjectTypeFrom(header);
	}
}

const QByteArray LooseStorage::objectDataFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->objectData.contains(actualId)) {
		kDebug() << "Loading data for" << actualId;

		QByteArray rawData = rawDataFor(actualId);

		d->objectData[actualId] = rawData.mid(RawObject::extractHeaderForm(rawData).size()+1);
	}

	return d->objectData[actualId];
}

RawObject* LooseStorage::objectFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->objects.contains(actualId)) {
		kDebug() << "load object" << actualId;
		d->objects[actualId] = RawObject::newInstance(actualId, repo());
	}

	return d->objects[actualId];
}

int LooseStorage::objectSizeFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	loadHeaderDataFor(actualId);

	return d->objectSizes[actualId];
}

ObjectType LooseStorage::objectTypeFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	loadHeaderDataFor(actualId);

	return d->objectTypes[actualId];
}

const QByteArray LooseStorage::rawDataFor(const QString &id, const qint64 maxRead)
{
	kDebug() << "Loading raw data for" << id;

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

const QString LooseStorage::sourceFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	Q_ASSERT(id.size() >= 7);
	QString idDirPath = actualId.left(2);
	QString idFilePath = actualId.mid(2);

	if (id.size() < 40) {
		QStringList possibleFileIds = QDir(d->objectsDir.filePath(idDirPath)).entryList(QStringList() << QString("%1*").arg(idFilePath));
		Q_ASSERT(possibleFileIds.size() == 1);
		idFilePath = possibleFileIds.first();
	}

	QString sourcePath = d->objectsDir.filePath("%1/%2").arg(idDirPath).arg(idFilePath);
	return sourcePath;
}



#include "LooseStorage.moc"
