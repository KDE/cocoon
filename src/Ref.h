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

#ifndef REF_H
#define REF_H

#include <QObject>

#include <QDir>
#include <QList>



class HeadTest;

namespace Git {

class Commit;
class Ref;
class Repo;

typedef QList<Ref*> RefList;



class Ref : public QObject
{
	Q_OBJECT

	public:
		Commit* commit() const;
		const QString& name() const;

		RefList all() const;

	protected:
		explicit Ref(const QString &prefix, const Repo &repo);
		explicit Ref(const QString &name, const QString &prefix, const Repo &repo);

		virtual Ref* newInstance(const QString &name, const Repo &repo) const = 0;
		virtual void populate();

	private:
		Commit *m_commit;
		QString m_name;
		QDir m_refsDir;
		const Repo &m_repo;

	friend class ::HeadTest;
};

}

#endif // REF_H
