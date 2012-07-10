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
 * @brief Cocoon's main window containing the main tabs.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

namespace LibQGit2 {
	class QGitRepository;
}
using namespace LibQGit2;

namespace Ui {
	class MainWindow;
}



/**
 * @brief Cocoon's main window containing the main tabs.
 */
class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

		void setRepository(const QString &repoPath);

	signals:
		void repositoryChanged(QGitRepository*);

	protected:
		void changeEvent(QEvent *e);

	protected slots:
		void open();
		void reload();

	private:
		void setupActions();

	private:
		QGitRepository *m_repo;
		Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
