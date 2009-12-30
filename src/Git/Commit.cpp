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

#include "Commit.h"

#include "Repo.h"

#include <KDebug>

#include <QStringList>

using namespace Git;



Commit::Commit(const Repo *repo)
	: QObject((QObject*)repo)
	, m_repo(repo)
{
}

const QString& Commit::author() const
{
	return m_author;
}

const KDateTime& Commit::authoredAt() const
{
	return m_authoredAt;
}

const QString& Commit::committer() const
{
	return m_committer;
}

const KDateTime& Commit::committedAt() const
{
	return m_committedAt;
}

CommitList Commit::fromRawRevList(const Repo *repo, QStringList &lines)
{
	CommitList commits;

	while(!lines.isEmpty()) {
		Commit *newCommit = new Commit(repo);

		newCommit->m_id   = lines.takeFirst().mid(strlen("commit "), -1);
		newCommit->m_tree = lines.takeFirst().mid(strlen("tree "), -1);
		while (lines.first().startsWith("parent ")) {
			newCommit->m_parents << lines.takeFirst().mid(strlen("parent "), -1);
		}
		QRegExp actorRegExp("^(.*) (\\d+) ([+-]\\d+)$");

		QString authorString = lines.takeFirst().mid(strlen("author "), -1);
		actorRegExp.indexIn(authorString);
		newCommit->m_author = actorRegExp.cap(1);
		newCommit->m_authoredAt.setTime_t(actorRegExp.cap(2).toInt()); // UTC time
		/** @todo add zone offset */

		QString committerString = lines.takeFirst().mid(strlen("committer "), -1);
		actorRegExp.indexIn(committerString);
		newCommit->m_committer = actorRegExp.cap(1);
		newCommit->m_committedAt.setTime_t(actorRegExp.cap(2).toInt()); // UTC time
		/** @todo add zone offset */

		lines.removeFirst();
		while (!lines.isEmpty() && lines.first().startsWith("    ")) {
			if (newCommit->m_message.isNull()) {
				newCommit->m_message = lines.takeFirst().mid(4, -1);
			} else {
				newCommit->m_message += "\n" + lines.takeFirst().mid(4, -1);
			}
		}
		newCommit->m_summary = newCommit->m_message.split("\n")[0];

		while (!lines.isEmpty() && lines.first().isEmpty())  {
			lines.removeFirst();
		}

		commits << newCommit;
	}

	return commits;
}

const QString& Commit::id() const
{
	return m_id;
}

const QString& Commit::message() const
{
	return m_message;
}

const QStringList& Commit::parents() const
{
	return m_parents;
}

const QString Commit::shortId() const
{
	return m_id.left(7);
}

const QString& Commit::summary() const
{
	return m_summary;
}

const QString& Commit::tree() const
{
	return m_tree;
}

#include "Commit.moc"
