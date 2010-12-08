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
 * @short A Git commit object.
 */

#ifndef COMMIT_H
#define COMMIT_H

#include "RawObject.h"

#include <KDateTime>

#include <QStringList>



class CommitListingTest;
class CommitMergeDetectionTest;
class CommitPopulationTest;
class CommitPopulationErrorsTest;

namespace Git {

class Commit;
class CommitPrivate;
class Ref;
class Repo;
class Tree;



/**
 * @short A Git commit object.
 */
class KDE_EXPORT Commit : public RawObject
{
	Q_OBJECT

	public:
		/**
		 * @brief Constructs an invalid Commit.
		 *
		 * It does not exist and has no SHA1.
		 * It is intended for indicating an error when returning objects.
		 *
		 * @sa isValid()
		 */
		explicit Commit();

		/**
		 * @brief Constructs a Git commit object.
		 *
		 * To speed up construction the commit will not be fully loaded until the first attribute access.
		 *
		 * @param id The commit's id.
		 * @param repo The repo the commit is in.
		 *
		 * @see fillFromString()
		 */
		explicit Commit(const Id& id, Repo &repo);

		/**
		 * @brief Copy constructor.
		 */
		Commit(const Commit &other);

		virtual ~Commit();



		const QString&     author();
		const KDateTime&   authoredAt();
		const QString&     committer();
		const KDateTime&   committedAt();

		/**
		 * @brief Returns the diff to the parent commit.
		 *
		 * @return The diff output.
		 *
		 * @note It will only  rpoduce the diff to the first parent.
		 *
		 * @todo Does fail on the first commit.
		 */
		const QString diff() const;

		/**
		 * @brief Find the children of this commit on the given branches.
		 *
		 * @param refs The branches (ids or names) to be checked for children.
		 *             If it is empty the current head is assumed.
		 * @return The list of child commits.
		 *
		 * @see
		 */
		QList<Commit> childrenOn(const QStringList &refs) const;

		/**
		 * @brief Has this commit been branched off of on the given branches?
		 *
		 * It has been if it has multiple children.
		 *
		 * @param refs The branches (ids or names) to be checked for children.
		 *             If empty the current head is assumed.
		 * @return true if it has multiple children on the given branches, false otherwise.
		 *
		 * @see childrenOn()
		 */
		bool hasBranchedOn(const QStringList &refs) const;

		/**
		 * @brief Is this a merge commit?
		 *
		 * It is if it has multiple parents.
		 *
		 * @return true if the commit has multiple parents, false otherwise.
		 *
		 * @see parents()
		 */
		bool isMerge();

		/**
		 * @brief Assigns @a other to @a this.
		 *
		 * @param other The commit to be assigned from.
		 * @return @a this.
		 */
		Commit& operator=(const Commit &other);

		/**
		 * @brief Returns a list of the commit's parents.
		 *
		 * @see isMerge()
		 */
		const QList<Commit> parents();

		/**
		 * @brief Returns the commit message.
		 *
		 * @return The commit message.
		 *
		 * @see summary()
		 */
		const QString& message();

		/**
		 * @brief Returns the first line of the commit message.
		 *
		 * @return The first line of the commit message.
		 *
		 * @see message()
		 */
		const QString& summary();

		/**
		 * @brief Returns the commit's tree.
		 *
		 * @return The tree.
		 */
		const Tree tree();

	// static
		static QList<Commit> allReachableFrom(const Ref &branch);

	private:
	// static
		/**
		 * @brief Finds the children of commit starting from the given list of refs.
		 *
		 * @param commit The commit to find the children of.
		 * @param refs Refs (ids or names) to start looking for them.
		 * @return The list of child commit ids.
		 */
		static QStringList childrenOf(const Commit &commit, const QStringList &refs);

		/**
		 * @brief Populates the given commit with the data extracted from the raw data.
		 *
		 * @param commit The commit to be populated.
		 * @param raw The raw commit data.
		 *
		 * @see data()
		 */
		static void fillFromString(Commit *commit, const QString &raw);

		static Commit latestIn(const QList<Commit> &commits);

		/**
		 * @short Parses zone offsets from string and returns the offset in seconds.
		 *
		 * It will parse +/-xx, +/-xxxx and +/-xx:xx offset formats.
		 *
		 * @code
		 *   parseZoneOffset("-0230"); // -2,5h offset
		 *   // will return -9000
		 * @endcode
		 *
		 * @param zoneOffsetString The zone offset string.
		 * @return The zone offset in seconds.
		 */
		static int parseZoneOffset(const QString &zoneOffsetString);

	private:
		QExplicitlySharedDataPointer<CommitPrivate> d;

	friend class ::CommitListingTest;
	friend class ::CommitMergeDetectionTest;
	friend class ::CommitPopulationTest;
	friend class ::CommitPopulationErrorsTest;
};

}

#endif // COMMIT_H
