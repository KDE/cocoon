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
 * @brief A Git tree object.
 */

#ifndef TREE_H
#define TREE_H

#include "RawObject.h"

#include <QHash>
#include <QMap>



class TreeTest;

namespace Git {

class Blob;
class TreePrivate;



/**
 * @brief A Git tree object.
 */
class KDE_EXPORT Tree : public RawObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructs an invalid Tree.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when returning objects.
		 *
		 * @sa isValid()
		 */
		explicit Tree();

		/**
		 * @brief Constructs a Git tree object.
		 *
		 * To speed up construction the tree will not be fully loaded until the first attribute access.
		 *
		 * @param id The tree's id.
		 * @param repo The repo the tree is in.
		 *
		 * @see fillFromString()
		 */
		explicit Tree(const Id& id, Repo &repo);

		/**
		 * @brief Copy constructor.
		 */
		Tree(const Tree &other);

		/**
		 * @brief Base class copy constructor.
		 */
		explicit Tree(const RawObject &other);

		virtual ~Tree();

		/**
		 * @brief Returns a list of all the blobs in this tree.
		 *
		 * @return The list of blobs.
		 *
		 * @see entries()
		 */
		const QList<Blob> blobs() const;

		/**
		 * @brief Returns a map of all the blobs with their entry names mapped to the associated objects.
		 *
		 * @return A map of name to blob.
		 *
		 * @see blobs(), nameFor()
		 */
		const QMap<QString, Blob> blobsByName() const;

		/**
		 * @brief Returns a list of all entries in this tree.
		 *
		 * Although it returns a list of objects these are only blobs and trees.
		 *
		 * @return The list of entries.
		 *
		 * @see blobs(), trees()
		 */
		const QList<RawObject> entries() const;

		/**
		 * @brief Returns a map of entry names with their associated objects.
		 *
		 * @return A map of name to object.
		 *
		 * @see entries(), nameFor()
		 */
		const QMap<QString, RawObject> entriesByName() const;

		/**
		 * @brief Returns the name that is associated with the given object in this tree.
		 *
		 * @param object The object to get the name for.
		 * @return The name of the object if it is an entry in this tree, an empty string otherwise.
		 *
		 * @see entries()
		 */
		const QString& nameFor(const RawObject &object) const;
		const QString& nameFor(const Id &id) const;

		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The tree to be assigned from.
		 * @return @a this.
		 */
		Tree& operator=(const Tree &other);

		/**
		 * @brief Returns a list of all the (sub-)trees in this tree.
		 *
		 * @return The list of trees.
		 *
		 * @see entries()
		 */
		const QList<Tree> trees() const;

		/**
		 * @brief Returns a map of all the trees with their entry names mapped to the associated objects.
		 *
		 * @return A map of name to tree.
		 *
		 * @see nameFor(), trees()
		 */
		const QMap<QString, Tree> treesByName() const;

	//static
		/**
		 * Returns an invalid Tree.
		 *
		 * @return An invalid Tree.
		 *
		 * @see isValid()
		 */
		static Tree& invalid();

	private:
		/**
		 * @brief Populates the tree with the data extracted from the raw data.
		 *
		 * @param raw The raw tree data.
		 *
		 * @see data()
		 */
		void fillFromString(const QByteArray &raw);

		/**
		 * @brief Will lazy load this tree's data and fill it in.
		 *
		 * @see fillFromString()
		 */
		void lazyLoad();

	private:
		QExplicitlySharedDataPointer<TreePrivate> d;

	friend class ::TreeTest;
};

}

#endif // TREE_H
