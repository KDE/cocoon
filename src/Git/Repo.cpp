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

#include "Repo.h"

#include <QDir>

using namespace Git;



Repo::Repo(QObject *parent)
	: QObject(parent)
{
}

bool Repo::containsRepository(const QString &path, const RepoOptions options)
{
	QString repoPath = QDir(path).absolutePath();

	QString bareRepoPath;
	QString dotGitPath;

	if (!(options & Bare)) {
		dotGitPath = repoPath + "/.git";
		if (QDir(dotGitPath).exists()) {
			bareRepoPath = dotGitPath;
		}
	}

	if (bareRepoPath.isNull()) {
		bareRepoPath = repoPath;
	}

	QDir bareRepoDir = QDir(bareRepoPath);
	if(bareRepoDir.exists()) { // "*.git" directory exists
		QStringList dirs, files; // it should contain these dirs and files
		dirs << "branches" << "objects" << "refs";
		files << "config" << "description" << "index";

		QStringList entries = bareRepoDir.entryList();

		bool isRepo = true; // will become false if only one item is missing

		/** @todo actually distinguish between dirs and files */
		foreach(const QString &item, dirs + files) {
			isRepo = isRepo && entries.contains(item);
		}

		return isRepo;
	}

	return false;
}

#include "Repo.moc"
