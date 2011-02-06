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

/**
 * @file
 * @author Riyad Preukschas <riyad@informatik.uni-bremen.de>
 * @brief The base for all Git refs as found in a repo's refs/ directory.
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



/**
 * @brief The base for all Git refs as found in a repo's refs/ directory.
 */
class KDE_EXPORT Ref : public QObject
{
	Q_OBJECT

	public:
		explicit Ref();

		/**
		 * @brief Copy constructor.
		 */
		Ref(const Ref &other);
		explicit Ref(Repo &repo);
		explicit Ref(const QString &remote, const QString &prefix, const QString &name, Repo &repo);


		QList<Ref> all(const QString &remote, const QString &prefix) const;
		QList<Ref> allHeads() const;

		/**
		 * @brief Returns the commit the ref points to.
		 *
		 * @return The commit.
		 */
		Commit commit() const;
		const QString fullName() const;
		bool isRemote() const;

		/**
		 * @brief Checks whether the ref is valid.
		 *
		 * The ref is invalid when it was constructed using the default constructor.
		 *
		 * @return true in case the ref is valid, false otherwise
		 *
		 * @see Ref()
		 */
		bool isValid() const;

		/**
		 * @brief The name of the ref.
		 *
		 * It will return:
		 * - "master" for "refs/heads/master"
		 * - "release-1.5" for "refs/tags/release-1.5"
		 * - "feature-x" for "refs/remotes/origin/feature-x"
		 *
		 * @return The name of the ref.
		 */
		const QString& name() const;

		/**
		 * @brief The prefix of the ref.
		 *
		 * It will return:
		 * - "heads" for "refs/heads/master"
		 * - "tags" for "refs/tags/release-1.5"
		 * - "origin" for "refs/remotes/origin/feature-x"
		 *
		 * @return The prefix of the ref.
		 */
		const QString& prefix() const;

		/**
		 * @brief The remote of the ref.
		 *
		 * It will return:
		 * - "" for "refs/heads/master"
		 * - "" for "refs/tags/release-1.5"
		 * - "remotes" for "refs/remotes/origin/feature-x"
		 *
		 * @return The remote of the ref.
		 */
		const QString& remote() const;

		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The ref to be assigned from.
		 * @return @a this.
		 */
		Ref& operator=(const Ref &other);

		/**
		 * @brief Compares two refs for equality.
		 *
		 * The comparison is based on the ref's prefix and name.
		 *
		 * @param other The ref to be compared to.
		 * @return true if both prefixes and names are equal, false otherwise
		 */
		bool operator==(const Ref &other) const;

	public: //static
		static bool exists(const QString &name, const Repo &repo);

		/**
		 * @brief Looks up a refs full name in a given repo.
		 *
		 * It will look in the repo's git directory for refs with that name.
		 *
		 * Examples:
		 * @code
		 *   Ref::fullNameFor("master", repo);
		 *   // will return "refs/heads/master"
		 *   Ref::fullNameFor("release-1.5", repo);
		 *   // will return "refs/tags/release-1.5"
		 *   Ref::fullNameFor("origin/feature-x", repo);
		 *   // will return "refs/remotes/origin/feature-x"
		 * @endcode
		 *
		 * @param name The short name of the ref.
		 * @param repo The repo to look in.
		 * @return The full ref path.
		 *
		 * @see http://www.kernel.org/pub/software/scm/git/docs/git-rev-parse.html#_specifying_revisions
		 */
		static QString fullNameFor(const QString &name, const Repo &repo);

		static Ref head(const QString &name, Repo &repo);

	protected:
		void populate();
		Repo& repo() const;

	private:
		QExplicitlySharedDataPointer<RefPrivate> d;

	friend class ::HeadTest;
};

}

#endif // REF_H
