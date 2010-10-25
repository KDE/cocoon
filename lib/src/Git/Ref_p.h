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

#ifndef REF_P_H
#define REF_P_H

#include "Id.h"
#include "Repo.h"

#include <QDir>



namespace Git {



class RefPrivate : public QSharedData {
public:
	RefPrivate()
		: QSharedData()
		, commitId()
		, name()
		, prefix()
		, refsDir()
		, remote()
		, repo(0)
	{}
	RefPrivate(const RefPrivate &other)
		: QSharedData()
		, commitId(other.commitId)
		, name(other.name)
		, prefix(other.prefix)
		, refsDir(other.refsDir)
		, remote(other.remote)
		, repo(other.repo)
	{}
	~RefPrivate() {}

	Id       commitId;
	QString  name;
	QString  prefix;
	QDir     refsDir;
	QString  remote;
	Repo    *repo;
};

};

#endif // REF_P_H
