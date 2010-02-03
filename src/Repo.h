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
#include "Head.h"
#include "Status.h"



class RepoCommitsCachingTest;
class RepoHeadsCachingTest;
class RepoStatusCachingTest;

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
		virtual ~Repo();

		void commitIndex(const QString &message, const QStringList &options = QStringList());
		CommitList commits(const QString &branch = QString());
		QString diff(const Commit &a, const Commit &b) const;
		QString head() const;
		RefList heads() const;
		const QString& gitDir() const;
		/** Stages files to be included in the next commit. */
		void stageFiles(const QStringList &paths);
		Status* status();
		/** Unstages (staged) files to not be included in the next commit. */
		void unstageFiles(const QStringList &paths);
		const QString& workingDir() const;

	// static
		static void clone(const QString &fromRepo, const QString &toDirectory, const QStringList &options = QStringList());
		static bool containsRepository(const QString &path);
		static void init(const QString &newRepoPath);

	public slots:
		void reset();
		void resetCommits();
		void resetHeads();
		void resetStatus();

	signals:
//		void currentHeadChanged();
		void headsChanged();
//		void headChanged(const QString&);
		void historyChanged();
		void indexChanged();

	private:
		class Private {
		public:
			RefList heads;
		};
		Private *d;
		QHash<QString, CommitList> m_commits;
		QString m_gitDir;
		Status *m_status;
		QString m_workingDir;

		friend class ::RepoCommitsCachingTest;
		friend class ::RepoHeadsCachingTest;
		friend class ::RepoStatusCachingTest;
};

}

#endif // REPO_H
