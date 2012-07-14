/*
	Cocoon - A GUI for Git.
	Copyright (C) 2012  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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
 * @brief A widget to show a formatted diff output.
 */

#ifndef COMMITHELPER_H
#define COMMITHELPER_H

namespace LibQGit2 {
	class QGitCommit;
}
using namespace LibQGit2;

#include <QtCore/QString>



namespace Cocoon {

class CommitHelper {
public:
	/**
	 * @brief Returns the diff to the parent commit.
	 *
	 * @return The diff output.
	 *
	 * @note It will only  rpoduce the diff to the first parent.
	 *
	 * @todo Does fail on the first commit.
	 */
	static QString diffParent(const QGitCommit &commit);
};

}

#endif // COMMITHELPER_H
