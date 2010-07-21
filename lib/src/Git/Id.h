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

#include <kdemacros.h>

#include <QSharedDataPointer>



class IdTest;

namespace Git {

class IdPrivate;
class ObjectStorage;
class Repo;



class KDE_EXPORT Id : public QObject
{
	Q_OBJECT

	public:
		explicit Id();
		Id(const Id &other);
		explicit Id(const QString &sha1, Repo &repo);
		explicit Id(const QString &sha1, ObjectStorage &storage);
		virtual ~Id();

		bool exists() const;
		bool isIn(const ObjectStorage &storage) const;
		bool isValid() const;
		ObjectStorage& storage() const;
		const QByteArray toBinarySha1() const;
		const QString&   toSha1String() const;
		const QString    toShortSha1String() const;
		const QString    toString() const;

		bool operator<(const Id &other) const;
		Id&  operator=(const Id &id);
		bool operator==(const Id &other) const;

	private:
		QSharedDataPointer<IdPrivate> d;

	friend class ::IdTest;
};

}



KDE_EXPORT QDataStream& operator<<(QDataStream &stream, const Git::Id &id);
KDE_EXPORT uint qHash(const Git::Id &id);



#endif // ID_H
