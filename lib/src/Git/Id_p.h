/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#ifndef ID_P_H
#define ID_P_H

#include <QSharedData>




namespace Git {

class ObjectStorage;



class IdPrivate : public QSharedData {
public:
	IdPrivate()
		: QSharedData()
		, repo(0)
		, sha1()
		, storage(0)
	{}
	IdPrivate(const IdPrivate &other)
		: QSharedData(other)
		, repo(other.repo)
		, sha1(other.sha1)
		, storage(other.storage)
	{}
	~IdPrivate() {}

	Repo          *repo;
	QString        sha1;
	ObjectStorage *storage;
};

}

#endif // ID_P_H
