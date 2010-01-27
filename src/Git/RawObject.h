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

#ifndef RAWOBJECT_H
#define RAWOBJECT_H

#include <QObject>



class RawObjectTest;

namespace Git {

class ObjectStorage;
class Repo;



class RawObject : public QObject
{
	Q_OBJECT

	public:
		RawObject(const QString& id, ObjectStorage *storage);
		RawObject(const QString& id, const QByteArray &rawData, ObjectStorage *storage);

		const QByteArray&  data() const;
		const QString&     id() const;
		const QString&     type() const;

//		bool hasValidHeader() const;
//		bool isValid() const;

	protected:
		virtual void populateWith(const QByteArray &rawData);

	private:
		const QString extractHeaderForm(const QByteArray &rawData) const;
		int extractObjectSizeFrom(const QString &header) const;
		const QString extractObjectTypeFrom(const QString &header) const;
		bool isValidHeader(const QString &possibleHeader) const;

	private:
		QByteArray     m_data;
		QString        m_id;
		int            m_size;
		ObjectStorage *m_storage;
		QString        m_type;

	friend class ::RawObjectTest;
};

}

#endif // RAWOBJECT_H
