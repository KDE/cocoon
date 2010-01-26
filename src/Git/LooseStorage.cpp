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

#include <KFilterDev>
#include <KMimeType>

#include <QDebug>

using namespace Git;



LooseStorage::LooseStorage(Repo *repo)
	: QObject(repo)
	, m_repo(repo)
{
	m_objectsDir = QDir(m_repo->workingDir());
	m_objectsDir.cd(".git/objects");

	qDebug() << "Objects dir:" << m_objectsDir;
}



const QByteArray LooseStorage::dataFor(const QString &id)
{
	QFile objectFile(sourceFor(id));

	Q_ASSERT(objectFile.exists());

	QByteArray rawData;
/*
	objectFile.open(QFile::ReadOnly);
	rawData = objectFile.readAll();
	objectFile.close();
*/

	KMimeType::Ptr mimeType = KMimeType::findByFileContent(objectFile.fileName());
	qDebug() << "Object mimetype is" << mimeType->name();

	QIODevice *filterDev = (KFilterDev *)KFilterDev::device(&objectFile, "application/x-gzip", false);
	Q_ASSERT(filterDev);

	static_cast<KFilterDev *>(filterDev)->setSkipHeaders();
	bool ok = filterDev->open(QIODevice::ReadOnly);
	Q_ASSERT(ok);
	rawData = filterDev->readAll();

	delete filterDev;

	return rawData;
}

const QString LooseStorage::sourceFor(const QString &id)
{
	Q_ASSERT(id.size() == 40);
	const QString objectPath = m_objectsDir.filePath("%1/%2").arg(id.left(2)).arg(id.mid(2));

	qDebug() << "Object" << id << "in" << objectPath;

	return objectPath;
}



#include "LooseStorage.moc"
