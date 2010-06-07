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

const QByteArray LooseStorage::rawDataFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->rawData.contains(actualId) || RawObject::isOnlyHeader(d->rawData[actualId])) {
		kDebug() << "Loading data for" << actualId;

		QFile objectFile(sourceFor(actualId));
		Q_ASSERT(objectFile.exists());

		// open the object's file
		bool ok = objectFile.open(QIODevice::ReadOnly);
		Q_ASSERT(ok);
		Q_UNUSED(ok);

		d->rawData[actualId] = inflate(objectFile.readAll());
	}

	return d->rawData[actualId];
}

const QByteArray LooseStorage::rawHeaderFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->rawData.contains(actualId)) {
		kDebug() << "Loading header for" << actualId;

		QFile objectFile(sourceFor(actualId));
		Q_ASSERT(objectFile.exists());

		// open the object's file
		bool ok = objectFile.open(QIODevice::ReadOnly);
		Q_ASSERT(ok);
		Q_UNUSED(ok);

		d->rawData[actualId] = RawObject::extractHeaderForm(inflate(objectFile.read(DEFLATED_HEADER_READ_SIZE))).toLatin1();
	}

	return d->rawData[actualId];
}

RawObject* LooseStorage::rawObjectFor(const QString &id)
{
	QString actualId = actualIdFor(id);

	if (!d->rawObjects.contains(actualId)) {
		kDebug() << "load object:" << actualId;
		d->rawObjects[actualId] = RawObject::newInstance(actualId, repo());
	}

	return d->rawObjects[actualId];
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
