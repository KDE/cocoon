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

#ifndef REPO_H
#define REPO_H

#include <QObject>

#include "Commit.h"



namespace Git {



enum RepoOption {
	None = 0,
	Bare = 1<<1
};
Q_DECLARE_FLAGS(RepoOptions, RepoOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(RepoOptions)



class Repo : public QObject
{
	Q_OBJECT

	public:
		explicit Repo(const QString &workingDir, QObject *parent = 0);

		CommitList commits() const;
		QString head() const;
		const QString& workingDir() const;

	// static
		static bool containsRepository(const QString &path);
		static void init(const QString &newRepoPath);

	private:
		QString m_workingDir;
};

}

#endif // REPO_H
