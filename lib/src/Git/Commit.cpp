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
	, d(new CommitPrivate)
{
}

Commit::Commit(const QString& id, Repo &repo)
	: RawObject(id, repo)
	, d(new CommitPrivate)
{
}



const QString& Commit::author()
{
	fillFromString(this, data());
	return d->author;
}

const KDateTime& Commit::authoredAt()
{
	fillFromString(this, data());
	return d->authoredAt;
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

const QString& Commit::committer()
{
	fillFromString(this, data());
	return d->committer;
}

const KDateTime& Commit::committedAt()
{
	fillFromString(this, data());
	return d->committedAt;
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
	// if commit has already been filled
	if (commit->d->tree || !commit->d->message.isEmpty()) {
		return;
	}

	QStringList lines = raw.split("\n");

	Tree *tree = 0;
	if (!lines.isEmpty() && lines.first().startsWith("tree ")) {
		QString treeId = lines.takeFirst().mid(qstrlen("tree "), -1);
		tree = commit->repo().tree(treeId);
	}
	commit->d->tree = tree;

	CommitList parents;
	while (!lines.isEmpty() && lines.first().startsWith("parent ")) {
		QString parentId = lines.takeFirst().mid(qstrlen("parent "), -1);
		parents << commit->repo().commit(parentId);
	}
	commit->d->parents = parents;

	QRegExp actorRegExp("^(.*) (\\d+) ([+-]\\d+)$");

	QString author;
	KDateTime authoredAt;
	if (!lines.isEmpty() && lines.first().startsWith("author ")) {
		QString zoneOffset;

		foreach (const QString part, lines.takeFirst().mid(qstrlen("author "), -1).split(" ")) {
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
	commit->d->author = author;
	commit->d->authoredAt = authoredAt;

	QString committer;
	KDateTime committedAt;
	if (!lines.isEmpty() && lines.first().startsWith("committer ")) {
		QString zoneOffset;

		foreach (const QString part, lines.takeFirst().mid(qstrlen("committer "), -1).split(" ")) {
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
	commit->d->committer = committer;
	commit->d->committedAt = committedAt;

	while (!lines.isEmpty() && lines.first().isEmpty()) {
		lines.removeFirst();
	}

	QString message;
	QString summary;
	while (!lines.isEmpty() && !lines.first().isEmpty()) {
		if (summary.isEmpty()) {
			summary = lines.takeFirst();
			message = summary;
		} else {
			message += "\n" + lines.takeFirst();
		}
	}
	commit->d->message = message;
	commit->d->summary = summary;

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

bool Commit::isMerge()
{
	return parents().size() > 1;
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

const QString& Commit::message()
{
	fillFromString(this, data());
	return d->message;
}

const CommitList Commit::parents()
{
	fillFromString(this, data());
	return d->parents;
}

const QString& Commit::summary()
{
	fillFromString(this, data());
	return d->summary;
}

const Tree* Commit::tree()
{
	fillFromString(this, data());
	return d->tree;
}

#include "Commit.moc"
