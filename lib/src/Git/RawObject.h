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
 * @brief The base for all Git objects.
 */

#ifndef RAWOBJECT_H
#define RAWOBJECT_H

#include <QObject>

#include "Id.h"

#include <kdemacros.h>

#include <QSharedDataPointer>



class RawObjectInstantiationTest;
class RawObjectTest;
class RawObjectTypeTest;

namespace Git {

class Blob;
class Commit;
class ObjectStorage;
class RawObjectPrivate;
class Repo;
class Tree;

typedef enum {
	OBJ_NONE = 0,
	OBJ_COMMIT = 1,
	OBJ_TREE = 2,
	OBJ_BLOB = 3,
	OBJ_TAG = 4,
//	OBJ_SOMETHING = 5, // reserved
	OBJ_OFS_DELTA = 6,
	OBJ_REF_DELTA = 7,
} ObjectType;



/**
 * @brief The base for all Git objects.
 */
class KDE_EXPORT RawObject : public QObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructs a basic Git object.
		 *
		 * This constructor should only be called from derived classes.
		 * If you need a general way to instantiate a Git object use newInstance().
		 *
		 * @see newInstance(), invalid()
		 */
		explicit RawObject(const Id& id, Repo &repo);

		/**
		 * @brief Constructs an invalid Git object.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when returning objects.
		 *
		 * @sa isValid()
		 */
		explicit RawObject();

		/**
		 * @brief Copy constructor.
		 */
		RawObject(const RawObject &other);

		virtual ~RawObject();



		/**
		 * @brief Returns the object's raw data.
		 *
		 * @return The object's raw data.
		 *
		 * @see ObjectStorage::objectDataFor()
		 */
		const QByteArray data();

		/**
		 * @brief Returns the object's id.
		 *
		 * @return The object's id.
		 */
		const Id& id() const;

		/**
		 * @brief Returns the object's data size.
		 *
		 * @return The object's data size.
		 *
		 * @see data()
		 */
		int size() const;

		/**
		 * @brief Returns the object's Git object type.
		 *
		 * An object can be a
		 * - Blob
		 * - Commit
		 * - Tag
		 * - Tree
		 *
		 * @return The object's type.
		 *
		 * @see isBlob(), isCommit(), isTag(), isTree()
		 */
		ObjectType type() const;

		/**
		 * @brief Returns whether this object is a blob.
		 *
		 * @return true if the object is a blob, false otherwise
		 *
		 * This is a convenience function.
		 * It is equvalent to
		 * @code
		 *   obj.type() == OBJ_BLOB
		 * @endcode
		 *
		 * @see Blob
		 */
		bool isBlob() const;

		/**
		 * @brief Returns whether this object is a commit.
		 *
		 * @return true if the object is a commit, false otherwise
		 *
		 * This is a convenience function.
		 * It is equvalent to
		 * @code
		 *   obj.type() == OBJ_COMMIT
		 * @endcode
		 *
		 * @see Commit
		 */
		bool isCommit() const;

		/**
		 * @brief Returns whether this object is a tag.
		 *
		 * @return true if the object is a tag, false otherwise
		 *
		 * This is a convenience function.
		 * It is equvalent to
		 * @code
		 *   obj.type() == OBJ_TAG
		 * @endcode
		 *
		 * @see Tag
		 */
		bool isTag() const;

		/**
		 * @brief Returns whether this object is a tree.
		 *
		 * @return true if the object is a tree, false otherwise
		 *
		 * This is a convenience function.
		 * It is equvalent to
		 * @code
		 *   obj.type() == OBJ_TREE
		 * @endcode
		 *
		 * @see Tree
		 */
		bool isTree() const;

		/**
		 * @brief Checks whether this Git object is valid.
		 *
		 * The Git object is invalid when it was constructed using the default constructor.
		 *
		 * @return true in case the Git object is valid, false otherwise
		 *
		 * @see RawObject(), invalid()
		 */
		bool isValid() const;

		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The Git object to be assigned from.
		 * @return @a this.
		 */
		RawObject& operator=(const RawObject &other);

		/**
		 * @brief Checks whether two Git objects are the same.
		 *
		 * @param other The object to compare to.
		 * @return true if both objects have the same id, false otherwise.
		 */
		bool operator==(const RawObject &other) const;

		/**
		 * Converts this Git object to a Blob.
		 *
		 * @return This Git object as a Blob or an invalid Blob.
		 *
		 * @see isBlob(), isValid()
		 */
		Blob toBlob() const;

		/**
		 * Converts this Git object to a Commit.
		 *
		 * @return This Git object as a Commit or an invalid Commit.
		 *
		 * @see isCommit(), isValid()
		 */
		Commit toCommit() const;

		/**
		 * Converts this Git object to a Tree.
		 *
		 * @return This Git object as a Tree or an invalid Tree.
		 *
		 * @see isTree(), isValid()
		 */
		Tree toTree() const;

	// static
		static const QString extractHeaderForm(const QByteArray &rawData);
		static int extractObjectSizeFrom(const QString &header);
		static ObjectType extractObjectTypeFrom(const QString &header);

		/**
		 * Returns an invalid RawObject.
		 *
		 * @return An invalid RawObject
		 *
		 * @see isValid()
		 */
		static RawObject& invalid();
		static bool isOnlyHeader(const QByteArray &rawData);
		static bool isValidHeader(const QString &possibleHeader);

		/**
		 * @deprecated
		 */
		static RawObject* newInstance(const Id& id, Repo &repo);
		static ObjectType typeFromTypeName(const QString& name);
		static const QString typeNameFromType(const ObjectType type);

	protected:
		virtual void populateHeader();
		Repo& repo() const;

	protected:
		QExplicitlySharedDataPointer<RawObjectPrivate> d;

	friend class Commit;
	friend class Tree;
	friend class ::RawObjectInstantiationTest;
	friend class ::RawObjectTest;
	friend class ::RawObjectTypeTest;
};

}

#endif // RAWOBJECT_H
