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

/**
 * @file
 * @author Riyad Preukschas <riyad@informatik.uni-bremen.de>
 * @short A Git object's id.
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
class RawObject;
class Repo;



/**
 * @short A Git object's id.
 *
 * An object's id is a SHA1 hash of its content.
 * It is also used to uniquely identify the object in the repository.
 *
 * You can create ids by supplying the full or a long enough (to be unique) part from the beginning of the id.
 * The full id will be looked up in the reposiory.
 *
 * @code
 *   Id("1234567890123456789012345678901234567890", repo)->toSha1String();
 *   // will return "1234567890123456789012345678901234567890"
 *   Id("1234567", repo)->toSha1String();
 *   // will return "1234567890123456789012345678901234567890"
 * @endcode
 */
class KDE_EXPORT Id : public QObject
{
	Q_OBJECT

	public:
		/**
		 * @short Constructs an invalid Id.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when retuning ids.
		 *
		 * @sa isValid()
		 */
		explicit Id();

		/**
		 * @short Copy constructor.
		 */
		Id(const Id &other);

		/**
		 * @short Constructs an object id.
		 *
		 * It will try to locate the storage the object is stored in.
		 * If necessary it will find the full id of the object.
		 *
		 * @note Ids should be recreated after moving objects between storages (e.g. a `git gc` run).
		 * @sa Id(const QString &sha1, ObjectStorage &storage)
		 */
		explicit Id(const QString &sha1, Repo &repo);
		explicit Id(const QString &sha1, ObjectStorage &storage);
		virtual ~Id();

		bool exists() const;
		bool isIn(const ObjectStorage &storage) const;

		/**
		 * @short Checks whether the id is valid.
		 *
		 * The id is invalid when it was constructed using the default constructor or when the object could not be found in the repo or was moved from the storage it was in once.
		 *
		 * @return true in case the id is valid, false otherwise
		 */
		bool isValid() const;
		RawObject* object() const;
		ObjectStorage& storage() const;
		const QByteArray toBinarySha1() const;
		const QString&   toSha1String() const;
		const QString    toShortSha1String() const;
		const QString&   toString() const;

		int compare(const Id &other) const;
		bool operator<(const Id &other) const;
		Id&  operator=(const Id &id);
		bool operator==(const Id &other) const;
		bool operator==(const QString &other) const;
		bool operator!=(const Id &other) const;

	private:
		QSharedDataPointer<IdPrivate> d;

	friend class ::IdTest;
};

}



KDE_EXPORT QDataStream& operator<<(QDataStream &stream, const Git::Id &id);
KDE_EXPORT uint qHash(const Git::Id &id);



#endif // ID_H
