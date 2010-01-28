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

#include "RawObject.h"
#include "Repo.h"

#include <KFilterBase>
#include <KMimeType>

#include <KDebug>

using namespace Git;



LooseStorage::LooseStorage(Repo *repo)
	: ObjectStorage(repo)
{
	m_objectsDir = QDir(repo->workingDir());
	m_objectsDir.cd(".git/objects");
}



const QByteArray LooseStorage::rawDataFor(const QString &id)
{
	if (!m_rawData.contains(id) || isOnlyHeader(m_rawData[id])) {
		kDebug() << "Loading data for" << id;

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
		const int bufferSize = 8*1024;

		// reserve memory
		inBuffer.resize(bufferSize);
		outBuffer.resize(bufferSize);

		while (result != KFilterBase::End || result != KFilterBase::Error) {
			// tell the filter about the out buffer
			filter->setOutBuffer(outBuffer.data(), outBuffer.size());

			if (filter->inBufferEmpty()) {
				// Request data from compressed file
				int readFromFile = filter->device()->read(inBuffer.data(), inBuffer.size());
				kDebug() << "Read" << readFromFile << "bytes from compressed file";

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

			int uncompressedBytes = outBuffer.size() - filter->outBufferAvailable();
			kDebug() << "Uncompressed" << uncompressedBytes << "bytes";

			// append the uncompressed data to the objects data
			rawData.append(outBuffer.data(), uncompressedBytes);

			if (result == KFilterBase::End) {
				kDebug() << "Finished unpacking";
				break; // Finished.
			}
		}

		delete filter;

		m_rawData[id] = rawData;
	}

	return m_rawData[id];
}

RawObject* LooseStorage::rawObjectFor(const QString &id)
{
	if (!m_rawObjects.contains(id)) {
		m_rawObjects[id] = new RawObject(id, this);
	}

	return m_rawObjects[id];
}

const QString LooseStorage::sourceFor(const QString &id)
{
	Q_ASSERT(id.size() == 40);
	return m_objectsDir.filePath("%1/%2").arg(id.left(2)).arg(id.mid(2));
}



#include "LooseStorage.moc"
