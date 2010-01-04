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

#ifndef COMMIT_H
#define COMMIT_H

#include <QObject>

#include <KDateTime>

#include <QStringList>



namespace Git {

class Commit;
class Repo;

typedef QList<Commit*>  CommitList;



class Commit : public QObject
{
	Q_OBJECT

	public:
		Commit(const Repo *repo);
		Commit(const QString& id, const Repo *repo);

		const QString&     author() const;
		const KDateTime&   authoredAt() const;
		const QString&     committer() const;
		const KDateTime&   committedAt() const;
		const QString      diff() const;
		const QString&     id() const;

		/**
		 * Is this a merge commit? It is if it has multiple parents.
		 *
		 * @return true if so, false otherwise.
		 */
		bool               isMerge() const;
		const QStringList& parents() const;
		const QString&     message() const;
		const QString      shortId() const;
		const QString&     summary() const;
		const QString&     tree() const;

	// static
		/**
		 * Creates a list of commits for the given repo from the the given raw log.
		 *
		 * @param repo The repository of the commits.
		 * @param rawLog The raw log as result of a `git log --pretty=raw` call.
		 * @return The list of commits extracted from the log.
		 */
		static CommitList fromRawLog(const Repo *repo, const QString &rawLog);

	private:
		/**
		 * Populates the given commit with the data extracted from the raw data.
		 *
		 * @param commit The commit to be populated.
		 * @param raw The raw commit data.
		 */
		static void fillFromString(Commit *commit, const QString &raw);

	private:
		QString     m_author;
		KDateTime   m_authoredAt;
		QString     m_committer;
		KDateTime   m_committedAt;
		QString     m_id;
		QString     m_message;
		QStringList m_parents;
		const Repo *m_repo;
		QString     m_summary;
		QString     m_tree;
};

}

#endif // COMMIT_H
