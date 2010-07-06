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

#ifndef ID_H
#define ID_H

#include <QObject>

#include "Id_p.h"



namespace Git {

class Repo;



class Id : public QObject
{
	Q_OBJECT

	public:
		explicit Id(const QString &sha1, Repo &repo);
		explicit Id(const QByteArray &binarySha1, Repo &repo);
		explicit Id(const Id &other);

		const QString& sha1() const;

	private:
		QSharedDataPointer<IdPrivate> d;
};

}



uint qHash(const Git::Id &id);



#endif // ID_H
