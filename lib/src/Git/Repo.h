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

#ifndef REPO_H
#define REPO_H

/**
 * @file
 * @author Riyad Preukschas <riyad@informatik.uni-bremen.de>
 * @brief A Git repository.
 */

#include <QObject>

#include "Commit.h"

#include <kdemacros.h>

#include <QSharedDataPointer>
#include <QStringList>

class RepoCommitsCachingTest;
class RepoHeadsCachingTest;
class RepoStatusCachingTest;

namespace Git {

class Blob;
class Commit;
class Id;
class ObjectStorage;
class RawObject;
class Ref;
class RepoPrivate;
class Status;
class Tree;



enum RepoOption {
	None = 0,
	Bare = 1<<1
};
Q_DECLARE_FLAGS(RepoOptions, RepoOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(RepoOptions)



/**
 * @brief A Git repository.
 */
class KDE_EXPORT Repo : public QObject
{
	Q_OBJECT

	public:
		explicit Repo(const QString &workingDir, QObject *parent = 0);
		Repo(const Repo &other);
		virtual ~Repo();

		Blob& blob(const Id &id);
		Commit& commit(const Id &id);

		/**
		 * @brief Returns the proper @link Git::Id Id @endlink object for a given string.
		 *
		 * This is a shorthand for @link Git::Id::Id(const QString&, Repo&) Id(QString&, Repo&) @endlink and should make your code more readable.
		 * @code
		 *   // instead of
		 *   Id("1234567", repo);
		 *   // you can now write
		 *   repo.idFor("1234567");
		 * @endcode
		 */
		Id idFor(const QString &idString);

		/**
		 * @deprecated Use Id::object() intead.
		 */
		RawObject& object(const Id &id);
		Tree& tree(const Id &id);

		void commitIndex(const QString &message, const QStringList &options = QStringList());
		QList<Commit> commits(const QString &branch = QString("HEAD"));
		const Ref& currentHead();
		QString diff(const Commit &a, const Commit &b) const;
		QList<Ref> heads();
		const QString& gitDir() const;
		const Ref& ref(const QString &name);
		/** Stages files to be included in the next commit. */
		void stageFiles(const QStringList &paths);
		Status* status();
		const QList<ObjectStorage*> storages();
		ObjectStorage* storageFor(const QString &id);
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
		void resetRefs();
		void resetLooseStorage();
		void resetPackedStorages();
		void resetStatus();

	signals:
//		void currentHeadChanged();
		void headsChanged();
//		void headChanged(const QString&);
		void historyChanged();

		/**
		 * @brief This is emitted when something changes the index.
		 *
		 * Actions that change the index are for example stage, unstage, commit, stash, etc.
		 */
		void indexChanged();

	private:
		QSharedDataPointer<RepoPrivate> d;

		friend class ::RepoCommitsCachingTest;
		friend class ::RepoHeadsCachingTest;
		friend class ::RepoStatusCachingTest;
};

}

#endif // REPO_H
