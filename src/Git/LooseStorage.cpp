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

#include <KFilterBase>
#include <KMimeType>

#include <KDebug>

using namespace Git;



LooseStorage::LooseStorage(Repo *repo)
	: QObject(repo)
	, m_repo(repo)
{
	m_objectsDir = QDir(m_repo->workingDir());
	m_objectsDir.cd(".git/objects");

	kDebug() << "Objects dir:" << m_objectsDir;
}



const QByteArray LooseStorage::rawDataFor(const QString &id)
{
	QFile objectFile(sourceFor(id));

	Q_ASSERT(objectFile.exists());

	QByteArray rawData;

	// get gzip filter for inflation and set it to work on the object's file
	KFilterBase *filter = KFilterBase::findFilterByMimeType("application/x-gzip");
	Q_ASSERT(filter);
	filter->setDevice(&objectFile, false);

	// open the object's file an init the fitler
	bool ok = objectFile.open(QIODevice::ReadOnly);
	Q_ASSERT(ok);
	filter->init(QIODevice::ReadOnly);

	// prepare for unpacking
	KFilterBase::Result result = KFilterBase::Ok;
	QByteArray inBuffer; // buffers reading the object's file
	QByteArray outBuffer; // buffers the uncompressed result
#define bufferSize  8*1024
/** @todo handle cases where data does not fit in buffer completely */

	// reserve memory
	inBuffer.resize(bufferSize);
	outBuffer.resize(bufferSize);

	while (result != KFilterBase::End || result != KFilterBase::Error) {
		// tell the filter about the out buffer
		filter->setOutBuffer(outBuffer.data(), outBuffer.size());

		if (filter->inBufferEmpty()) {
			// Request data from compressed file
			int readFromFile = filter->device()->read(inBuffer.data(), inBuffer.size());
			kDebug() << "got" << readFromFile << "bytes from compressed file";

			if (readFromFile) {
				// tell the filter about the new data
				filter->setInBuffer(inBuffer.data(), readFromFile);
			} else {
				kDebug() << "Not enough data available in object file for now";
				break;
			}
		}

		// no need for reading header

		// do the deed
		result = filter->uncompress();

		if (result == KFilterBase::Error) {
			kWarning() << "Error when uncompressing object" << id;
			break; // Error
		}

		kDebug() << "Uncompressed" << filter->outBufferAvailable() << "bytes";

		outBuffer.data()[filter->outBufferAvailable()] = '\0';
		kDebug() << "Uncompressed data:" << outBuffer;

		// append the uncompressed data to the objects data
		rawData.append(outBuffer.data(), filter->outBufferAvailable());

		if (result == KFilterBase::End) {
			kDebug() << "Finished unpacking";
			break; // Finished.
		}
	}

	kDebug() << "Uncompressed raw object data:" << rawData;

	delete filter;

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
