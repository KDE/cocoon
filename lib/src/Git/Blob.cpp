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

#include "Blob.h"

#include "RawObject_p.h"

using namespace Git;



Blob::Blob()
	: RawObject()
	, d(RawObject::d)
{
}

Blob::Blob(const Id& id, Repo &repo)
	: RawObject(id, repo)
	, d(RawObject::d)
{
}

Blob::Blob(const Blob &other)
	: RawObject(other)
	, d(RawObject::d)
{
}

Blob::Blob(const RawObject &other)
	: RawObject(other)
	, d(RawObject::d)
{
	Q_ASSERT(other.isBlob());
}

Blob::~Blob()
{
}



Blob& Blob::operator=(const Blob &other)
{
	RawObject::operator=(other);

	return *this;
}

#include "Blob.moc"
