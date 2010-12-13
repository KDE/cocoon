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
 * @brief A Git blob object.
 */

#ifndef BLOB_H
#define BLOB_H

#include "RawObject.h"



class BlobTest;

namespace Git {



/**
 * @brief A Git blobl object.
 */
class KDE_EXPORT Blob : public RawObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructs an invalid Blob.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when returning objects.
		 *
		 * @see isValid()
		 */
		explicit Blob();

		/**
		 * @brief Constructs a Git blob object.
		 *
		 * @param id The blob's id.
		 * @param repo The repo the blob is in.
		 */
		explicit Blob(const Id& id, Repo &repo);

		/**
		 * @brief Copy constructor.
		 */
		Blob(const Blob &other);

		/**
		 * @brief Base class copy constructor.
		 */
		explicit Blob(const RawObject &other);

	public:
		virtual ~Blob();



		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The blob to be assigned from.
		 * @return @a this.
		 */
		Blob& operator=(const Blob &id);

	private:
		QExplicitlySharedDataPointer<RawObjectPrivate> d;

	friend class ::BlobTest;
};

}

#endif // BLOB_H
