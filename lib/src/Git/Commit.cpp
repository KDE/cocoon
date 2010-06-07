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
#include "Ref.h"
#include "Repo.h"

#include <QStringList>

using namespace Git;



Commit::Commit(const QString &id, QObject *parent)
	: RawObject(id, parent)
{
}

Commit::Commit(const QString& id, Repo &repo)
	: RawObject(id, repo)
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
	runner.setDirectory(commit.repo().workingDir());

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
		actualRefs << repo().head()->name();
	}

	// used for caching the result
	static QHash<QString, CommitList> childrenByRefs;

	QString refKey = id() + ": " + actualRefs.join(" ");

	if (!childrenByRefs.contains(refKey)) {
		QStringList childrenIds = childrenOf(*this, actualRefs);

		CommitList children;
		foreach (const QString &id, childrenIds) {
			children << new Commit(id, repo());
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
	runner.setDirectory(repo().workingDir());
	runner.commitDiff(id());
	return runner.getResult();
}

void Commit::fillFromString(Commit *commit, const QString &raw)
{
	QStringList lines = raw.split("\n");

	Tree *tree = 0;
	if (!lines.isEmpty() && lines.first().startsWith("tree ")) {
		QString treeId = lines.takeFirst().mid(strlen("tree "), -1);
		tree = commit->repo().tree(treeId);
	}
	commit->m_tree = tree;

	CommitList parents;
	while (lines.first().startsWith("parent ")) {
		QString parentId = lines.takeFirst().mid(strlen("parent "), -1);
		parents << commit->repo().commit(parentId);
	}
	commit->m_parents = parents;

	QRegExp actorRegExp("^(.*) (\\d+) ([+-]\\d+)$");

	QString author;
	KDateTime authoredAt;
	if (!lines.isEmpty() && lines.first().startsWith("author ")) {
		QString zoneOffset;

		foreach (const QString part, lines.takeFirst().mid(strlen("author "), -1).split(" ")) {
			if (part.contains(QRegExp("^[+-]\\d{4}$"))) {
				zoneOffset = part;
			} else if (part.contains(QRegExp("^\\d{9,11}$"))) {
				authoredAt.setTime_t(part.toLong()); // UTC time
			} else {
				if (!author.isEmpty()) {
					author += " ";
				}
				author += part;
			}
		}

		/** @todo add zone offset */
	}
	commit->m_author = author;
	commit->m_authoredAt = authoredAt;

	QString committer;
	KDateTime committedAt;
	if (!lines.isEmpty() && lines.first().startsWith("committer ")) {
		QString zoneOffset;

		foreach (const QString part, lines.takeFirst().mid(strlen("committer "), -1).split(" ")) {
			if (part.contains(QRegExp("^[+-]\\d{4}$"))) {
				zoneOffset = part;
			} else if (part.contains(QRegExp("^\\d{9,11}$"))) {
				committedAt.setTime_t(part.toLong()); // UTC time
			} else {
				if (!committer.isEmpty()) {
					committer += " ";
				}
				committer += part;
			}
		}

		/** @todo add zone offset */
	}
	commit->m_committer = committer;
	commit->m_committedAt = committedAt;

	while (!lines.isEmpty() && lines.first().isEmpty()) {
		lines.removeFirst();
	}

	QString message;
	while (!lines.isEmpty() && lines.first().startsWith("    ")) {
		if (message.isEmpty()) {
			message = lines.takeFirst().mid(4, -1);
		} else {
			message += "\n" + lines.takeFirst().mid(4, -1);
		}
	}
	commit->m_message = message;
	commit->m_summary = message.split("\n")[0];

	while (!lines.isEmpty() && lines.first().isEmpty())  {
		lines.removeFirst();
	}
}


CommitList Commit::allReachableFrom(const Ref &ref)
{
	CommitList commits;

	CommitList fringe;
	fringe << ref.commit();

	while (!fringe.isEmpty()) {
		// find next commit in list
		Commit *commit = latestIn(fringe);

		// add parents to fringe
		foreach (Commit *parent, commit->parents()) {
			if (!fringe.contains(parent)) {
				fringe << commit->parents();
			}
		}

		// remove the commit from the fringe
		fringe.removeOne(commit);
		// and sort in it in
		commits << commit;
	}

	return commits;
}

bool Commit::hasBranchedOn(const QStringList &refs) const
{
	return childrenOn(refs).size() > 1;
}

bool Commit::isMerge() const
{
	return m_parents.size() > 1;
}

Commit* Commit::latestIn(const CommitList &commits)
{
	Commit *latest = commits.first();
	foreach (Commit *commit, commits) {
		if (commit->committedAt() > latest->committedAt()) {
			latest = commit;
		}
	}

	return latest;
}

const QString& Commit::message() const
{
	return m_message;
}

const CommitList Commit::parents() const
{
	return m_parents;
}

const QString& Commit::summary() const
{
	return m_summary;
}

const Tree* Commit::tree() const
{
	return m_tree;
}

#include "Commit.moc"
