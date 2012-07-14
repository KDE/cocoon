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
 * @brief The commit history tab.
 */

#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QtGui/QWidget>

namespace Ui {
	class HistoryWidget;
}

class GitBranchesModel;
class GitHistoryModel;

namespace LibQGit2 {
	class QGitRepository;
}
using namespace LibQGit2;

class QModelIndex;



/**
 * @brief The commit history tab.
 */
class HistoryWidget : public QWidget
{
	Q_OBJECT

	public:
		HistoryWidget(QWidget *parent = 0);
		~HistoryWidget();

	public slots:
		void setRepository(QGitRepository *repo);

	private:
		void clear();
		void loadModels();
		void showCurrentBranch();

	private slots:
		void on_branchComboBox_currentIndexChanged(const QString&);
		void on_historyView_clicked(const QModelIndex &index);

	private:
		GitBranchesModel *m_branchesModel;
		GitHistoryModel *m_historyModel;
		QGitRepository *m_repo;
		Ui::HistoryWidget *ui;
};

#endif // HISTORYWIDGET_H
