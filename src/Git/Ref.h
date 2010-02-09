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

#include "Ref_p.h"

#include <kdemacros.h>

#include <QSharedDataPointer>



class HeadTest;

namespace Git {

class Commit;
class Ref;
class RefPrivate;
class Repo;

typedef QList<Ref*> RefList;



class KDE_EXPORT Ref : public QObject
{
	Q_OBJECT

	public:
		Commit* commit() const;
		const QString& name() const;

		RefList all() const;

		Ref& operator=(const Ref &other);

	protected:
		explicit Ref(const QString &prefix, Repo &repo);
		explicit Ref(const QString &name, const QString &prefix, Repo &repo);
		Ref(const Ref &other);

		virtual Ref* newInstance(const QString &name, Repo &repo) const = 0;
		virtual void populate();

	private:
		QSharedDataPointer<RefPrivate> d;

	friend class ::HeadTest;
};

}

#endif // REF_H
