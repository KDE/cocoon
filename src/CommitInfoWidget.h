/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2012  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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
 * @brief A widget to show a commit's details.
 */

#ifndef COMITINFOWIDGET_H
#define COMITINFOWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
	class CommitInfoWidget;
}

namespace LibQGit2 {
	class QGitCommit;
	class QGitRepository;
}
using namespace LibQGit2;
#include <QGit2/QGitCommit>


/**
 * @brief A widget to show a commit's details.
 */
class CommitInfoWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit CommitInfoWidget(QWidget *parent = 0);
		~CommitInfoWidget();

		void setCommit(const QGitCommit &commit);

	public slots:
		void clear();
		void updateView();

	private:
		QGitCommit m_commit;
		Ui::CommitInfoWidget *ui;
};

#endif // COMITINFOWIDGET_H
