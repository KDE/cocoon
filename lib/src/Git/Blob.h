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

#ifndef BLOB_H
#define BLOB_H

#include "RawObject.h"



class BlobTest;

namespace Git {



class KDE_EXPORT Blob : public RawObject
{
	Q_OBJECT

	public:
		Blob(const QString& id, Repo &repo);

	protected:
		Blob(const QString& id, QObject *parent=0);

		friend class ::BlobTest;
};

}

#endif // BLOB_H
