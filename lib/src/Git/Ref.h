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

#include "Ref_p.h" // this should not be here, but there are strange compilation errors without it

#include <kdemacros.h>

#include <QSharedDataPointer>



class HeadTest;

namespace Git {

class Commit;
class Ref;
class RefPrivate;
class Repo;



class KDE_EXPORT Ref : public QObject
{
	Q_OBJECT

	public:
		explicit Ref();
		Ref(const Ref &other);
		explicit Ref(const QString &remote, const QString &prefix, const QString &name, Repo &repo);

		Commit* commit() const;
		const QString  fullName() const;
		bool isRemote() const;
		bool isValid() const;
		const QString& name() const;
		const QString& prefix() const;
		const QString& remote() const;

		QList<Ref> all() const;

		Ref& operator=(const Ref &other);
		bool operator==(const Ref &other) const;

	public: //static
		static bool exists(const QString &name, const Repo &repo);
		static QString fullNameFor(const QString &name, const Repo &repo);
		static Ref newInstance(const QString &remote, const QString &prefix, const QString &name, Repo &repo);

	protected:
		void populate();
		Repo& repo() const;

	private:
		QExplicitlySharedDataPointer<RefPrivate> d;

	friend class ::HeadTest;
};

}

#endif // REF_H
