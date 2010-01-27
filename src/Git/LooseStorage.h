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

#ifndef LOOSESTORAGE_H
#define LOOSESTORAGE_H

#include <QObject>

#include <QDir>

class LooseStorageTest;

namespace Git {

class Repo;



class LooseStorage : public QObject
{
	Q_OBJECT

	public:
		explicit LooseStorage(Repo *repo);

		const QByteArray rawDataFor(const QString &id);
		const QString sourceFor(const QString &id);

	protected:
		const QString extractHeaderForm(const QByteArray &rawData);
		const QString extractObjectTypeFrom(const QByteArray &rawData);

	private:
		QDir m_objectsDir;
		Repo *m_repo;

	friend class ::LooseStorageTest;
};

}

#endif // LOOSESTORAGE_H
