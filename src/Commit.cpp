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

#include "gitrunner.h"

#include "ObjectStorage.h"
#include "Repo.h"

#include <KDebug>

#include <QStringList>

using namespace Git;



Commit::Commit(const QString& id, ObjectStorage &storage)
	: RawObject(id, storage)
{
	fillFromString(this, data());
}

const QString& Commit::author() const
{
	return m_author;
}

const KDateTime& Commit::authoredAt() const
{
	return m_authoredAt;
}

QStringList Commit::childrenOf(const Commit &commit, const QStringList &refs)
{
	GitRunner runner;
	runner.setDirectory(commit.storage().repo().workingDir());

	QStringList opts;
	opts << "--children";

	QStringList commits;
	commits << refs;
	commits << QString("^%1^@").arg(commit.id());

	runner.revList(opts, commits);

	QStringList revList = runner.getResult().split("\n");
	revList.removeLast();
	int revIndexForCommit = revList.indexOf(QRegExp(QString("^%1 .*$").arg(commit.id())));
	if (revIndexForCommit != -1) {
		const QString &revLineForCommit = revList[revIndexForCommit];

		return revLineForCommit.split(" ").mid(1);
	} else {
		return QStringList();
	}
}

CommitList Commit::childrenOn(const QStringList &refs) const
{
	QStringList actualRefs(refs);
	if (actualRefs.isEmpty()) {
		actualRefs << storage().repo().head();
	}

	// used for caching the result
	static QHash<QString, CommitList> childrenByRefs;

	QString refKey = id() + ": " + actualRefs.join(" ");

	if (!childrenByRefs.contains(refKey)) {
		QStringList childrenIds = childrenOf(*this, actualRefs);

		CommitList children;
		foreach (const QString &id, childrenIds) {
			children << new Commit(id, storage());
		}

		childrenByRefs[refKey] = children;
	}

	return childrenByRefs[refKey];
}

const QString& Commit::committer() const
{
	return m_committer;
}

const KDateTime& Commit::committedAt() const
{
	return m_committedAt;
}

const QString Commit::diff() const
{
	GitRunner runner;
	runner.setDirectory(storage().repo().workingDir());
	runner.commitDiff(id());
	return runner.getResult();
}

void Commit::fillFromString(Commit *commit, const QString &raw)
{
	QStringList lines = raw.split("\n");

	commit->m_id   = lines.takeFirst().mid(strlen("commit "), -1);
	commit->m_tree = lines.takeFirst().mid(strlen("tree "), -1);
	while (lines.first().startsWith("parent ")) {
		commit->m_parents << commit->storage().repo().commit(lines.takeFirst().mid(strlen("parent "), -1));
	}
	QRegExp actorRegExp("^(.*) (\\d+) ([+-]\\d+)$");

	QString authorString = lines.takeFirst().mid(strlen("author "), -1);
	actorRegExp.indexIn(authorString);
	commit->m_author = actorRegExp.cap(1);
	commit->m_authoredAt.setTime_t(actorRegExp.cap(2).toInt()); // UTC time
	/** @todo add zone offset */

	QString committerString = lines.takeFirst().mid(strlen("committer "), -1);
	actorRegExp.indexIn(committerString);
	commit->m_committer = actorRegExp.cap(1);
	commit->m_committedAt.setTime_t(actorRegExp.cap(2).toInt()); // UTC time
	/** @todo add zone offset */

	lines.removeFirst();
	while (!lines.isEmpty() && lines.first().startsWith("    ")) {
		if (commit->m_message.isNull()) {
			commit->m_message = lines.takeFirst().mid(4, -1);
		} else {
			commit->m_message += "\n" + lines.takeFirst().mid(4, -1);
		}
	}
	commit->m_summary = commit->m_message.split("\n")[0];

	while (!lines.isEmpty() && lines.first().isEmpty())  {
		lines.removeFirst();
	}
}


CommitList Commit::allReachableFrom(const Ref &ref, const Repo &repo)
{
	CommitList commits;

	/** @todo generate list of commits for branch */

	return commits;
}

bool Commit::hasBranchedOn(const QStringList &refs) const
{
	return childrenOn(refs).size() > 1;
}

const QString& Commit::id() const
{
	return m_id;
}

bool Commit::isMerge() const
{
	return m_parents.size() > 1;
}

const QString& Commit::message() const
{
	return m_message;
}

const CommitList Commit::parents() const
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
