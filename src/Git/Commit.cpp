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

#include <QStringList>

using namespace Git;



Commit::Commit(const Repo *repo)
	: QObject((QObject*)repo)
	, m_repo(repo)
{
}

const QString& Commit::id() const {
	return m_id;
}

const QString Commit::shortId() const {
	return m_id.left(7);
}

const QString& Commit::message() const {
	return m_message;
}

CommitList Commit::fromRawRevList(const Repo *repo, QStringList &lines)
{
	CommitList commits;

	while(!lines.isEmpty()) {
		Commit *newCommit = new Commit(repo);

		newCommit->m_id = lines.takeFirst().mid(strlen("commit "), -1);
		lines.removeFirst(); // tree
		lines.removeFirst(); // parent
		lines.removeFirst(); // author
		lines.removeFirst(); // committer
		lines.removeFirst();
		while(!lines.isEmpty() && lines.first().startsWith("    ")) {
			if(newCommit->m_message.isNull()) {
				newCommit->m_message = lines.takeFirst().mid(4, -1);
			} else {
				newCommit->m_message += "\n" + lines.takeFirst().mid(4, -1);
			}
		}

		while(!lines.isEmpty() && lines.first().isEmpty())  {
			lines.removeFirst();
		}

		commits << newCommit;
	}

	return commits;
}

#include "Commit.moc"
