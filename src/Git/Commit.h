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

class QStringList;



namespace Git {

class Commit;
class Repo;

typedef QList<Commit*>  CommitList;



class Commit : public QObject
{
	Q_OBJECT

	public:
		Commit(const Repo *repo);
		Commit(const QString& treeish);

		const QString& id() const;
		const QString  shortId() const;
		const QString& message() const;

	// static
		static CommitList fromRawRevList(const Repo *repo, QStringList &lines);

	private:
		const Repo *m_repo;
		QString m_id;
		QString m_message;
};

}

#endif // COMMIT_H
