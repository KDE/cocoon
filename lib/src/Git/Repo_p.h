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

#ifndef REPO_P_H
#define REPO_P_H

#include "Commit.h"
#include "LooseStorage.h"
#include "Ref.h"
#include "Status.h"

namespace Git {

class RepoPrivate : public QSharedData {
public:
	RepoPrivate()
		: QSharedData()
		, commits()
		, gitDir()
		, refs()
		, looseStorage(0)
		, status(0)
		, storages()
		, workingDir()
	{}
	RepoPrivate(const RepoPrivate &other)
		: QSharedData()
		, commits(other.commits)
		, gitDir(other.gitDir)
		, refs(other.refs)
		, looseStorage(other.looseStorage)
		, status(other.status)
		, storages(other.storages)
		, workingDir(other.workingDir)
	{}
	~RepoPrivate() {}

	QHash<QString, QList<Commit> > commits;
	QString gitDir;
	QHash<QString, Ref> refs;
	LooseStorage *looseStorage;
	Status *status;
	QList<ObjectStorage*> storages;
	QString workingDir;
};

}

#endif // REPO_P_H
