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

#ifndef REPO_P_H
#define REPO_P_H

#include "Commit.h"
#include "Head.h"
#include "LooseStorage.h"
#include "Status.h"

namespace Git {

class RepoPrivate : public QSharedData {
public:
	RepoPrivate(Repo &repo)
		: QSharedData()
		, commits()
		, gitDir()
		, heads()
		, looseStorage(repo)
		, status(0)
		, workingDir()
	{}
	RepoPrivate(const RepoPrivate &other)
		: QSharedData()
		, commits(other.commits)
		, gitDir(other.gitDir)
		, heads(other.heads)
		, looseStorage(other.looseStorage)
		, status(other.status)
		, workingDir(other.workingDir)
	{}

	QHash<QString, CommitList> commits;
	QString gitDir;
	RefList heads;
	LooseStorage looseStorage;
	Status *status;
	QString workingDir;
};

};

#endif // REPO_P_H
