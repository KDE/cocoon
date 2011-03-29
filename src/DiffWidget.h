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
 * @brief A widget to show a formatted diff output.
 */

#ifndef DIFFWIDGET_H
#define DIFFWIDGET_H

#include <KTextBrowser>


/**
 * @brief A widget to show a formatted diff output.
 */
class DiffWidget : public KTextBrowser
{
	Q_OBJECT

	public:
		explicit DiffWidget(QWidget *parent = 0);

		void setDiff(const QString &diffString);

	private:
		const QString format(const QString &rawDiffString) const;
};

#endif // DIFFWIDGET_H
