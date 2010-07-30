/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2010  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#ifndef COMMIT_P_H
#define COMMIT_P_H

#include <QSharedData>

#include <KDateTime>



namespace Git {

class Commit;
class Tree;



class CommitPrivate : public QSharedData {
public:
	CommitPrivate()
		: QSharedData()
		, author()
		, authoredAt()
		, committer()
		, committedAt()
		, message()
		, parents()
		, summary()
		, tree(0)
	{}
	CommitPrivate(const CommitPrivate &other)
		: QSharedData(other)
		, author(other.author)
		, authoredAt(other.authoredAt)
		, committer(other.committer)
		, committedAt(other.committedAt)
		, message(other.message)
		, parents(other.parents)
		, summary(other.summary)
		, tree(other.tree)
	{}
	~CommitPrivate() {}

	QString   author;
	KDateTime authoredAt;
	QString   committer;
	KDateTime committedAt;
	QString   message;
	QList<Commit*> parents;
	QString   summary;
	Tree     *tree;
};

}

#endif // COMMIT_P_H
