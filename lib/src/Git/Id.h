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
 * @brief A Git object's id.
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
 * @brief A Git object's id.
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
 *
 * @see Repo::idFor()
 */
class KDE_EXPORT Id : public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructs an invalid Id.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when returning ids.
		 *
		 * @sa isValid()
		 */
		explicit Id();

		/**
		 * @brief Copy constructor.
		 */
		Id(const Id &other);

		/**
		 * @brief Constructs an object id.
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
		 * @brief Checks whether the id is valid.
		 *
		 * The id is invalid when it was constructed using the default constructor or when the object could not be found in the repo or was moved from the storage it was in once.
		 *
		 * @return true in case the id is valid, false otherwise
		 */
		bool isValid() const;

		/**
		 * @brief Returns the object represented by the id.
		 */
		RawObject& object() const;

		/**
		 * @brief Retruns the id's repo.
		 *
		 * @return The id's repo.
		 */
		Repo& repo() const;

		/**
		 * @brief Returns the storage the object with this id is stored in.
		 *
		 * @return The object's storage.
		 */
		ObjectStorage& storage() const;

		/**
		 * @brief Returns the id's SHA1 in its 20 byte binary form.
		 *
		 * @return The 20 byte binary SHA1.
		 */
		const QByteArray toBinarySha1() const;

		/**
		 * @brief Returns the id's SHA1 in 40 character string form.
		 *
		 * The SHA1 is is represented as a 40 character hexadecimal string.
		 *
		 * @return The 40 character SHA1 string.
		 *
		 * @see toShortSha1String()
		 */
		const QString& toSha1String() const;

		/**
		 * @brief Returns a shortened representation of the ids SHA1.
		 *
		 * It will give you the first 7 characters of the SHA1 string.
		 *
		 * @return The first 7 characters of the SHA1 string.
		 */
		const QString toShortSha1String() const;

		/**
		 * @brief Returns the id's SHA1 in 40 character string form.
		 *
		 * The SHA1 is is represented as a 40 character hexadecimal string.
		 *
		 * @return The 40 character SHA1 string.
		 *
		 * @see toSha1String()
		 */
		const QString& toString() const;

		int compare(const Id &other) const;

		/**
		 * @brief Compares two ids for numerical order.
		 *
		 * The comparison is based on the id's SHA1 value.
		 *
		 * @param other The other id.
		 * @return true if the other id is numerically greater, false otherwise
		 *
		 * @note This is needed for Id to be used as key in QHash.
		 */
		bool operator<(const Id &other) const;

		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The id to be assigned from.
		 * @return @a this.
		 */
		Id& operator=(const Id &id);

		/**
		 * @brief Compares two ids for equality.
		 *
		 * The comparison is based on the id's SHA1 value.
		 *
		 * @param other The id to be compared to.
		 * @return true if both SHA1s are equal, false otherwise
		 *
		 * @note This is needed for Id to be used as key in QHash.
		 */
		bool operator==(const Id &other) const;

		/**
		 * @brief Compares an id with a string for equality.
		 *
		 * This is equivalent to:
		 * @code
		 *   id.toSha1String() == other;
		 * @endcode
		 *
		 * The comparison is based on the id's full SHA1 value.
		 * @code
		 *   Id("1234567", repo) == "1234567890123456789012345678901234567890";
		 *   // will return true
		 *   Id("1234567", repo) == "1234567";
		 *   // will always be false
		 * @endcode
		 *
		 * @param other The string to be compared to.
		 * @return true if the id's SHA1s and the string are equal, false otherwise
		 */
		bool operator==(const QString &other) const;

		/**
		 * @brief Compares two ids for inequality.
		 *
		 * The comparison is based on the id's SHA1 value.
		 *
		 * @param other The id to be compared to.
		 * @return true if both SHA1s are not equal, false otherwise
		 */
		bool operator!=(const Id &other) const;

	private:
		QSharedDataPointer<IdPrivate> d;

	friend class ::IdTest;
};

}



/**
 * @brief Writes an Id to a stream.
 *
 * This is a convenience function so you can use Id directly in QDataStream.
 * @code
 *   qDebug() << "Can't load object with id" << id;
 * @endcode
 *
 * @param stream The stream to write to.
 * @param id The id to be written.
 * @return A reference to the stream.
 *
 * @see #Id::toSha1String()
 */
KDE_EXPORT QDataStream& operator<<(QDataStream &stream, const Git::Id &id);


/**
 * @brief Computes a hash key from the Id.
 *
 * This function would be used by QHash if you wanted to build a hash table for instances of Id.
 * As the id is already the hash value of the object it represents, the first bytes of the id are used as hash key here.
 *
 * @param id The id.
 * @return The hash key.
 *
 * @note This is needed for Id to be used as key in QHash.
 */
KDE_EXPORT uint qHash(const Git::Id &id);



#endif // ID_H
