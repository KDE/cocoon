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
 * @brief The commit history comparison tab.
 */

#ifndef COMPAREWIDGET_H
#define COMPAREWIDGET_H

#include <QWidget>

#include "Git/Id.h"

namespace Git {
	class Repo;
}

namespace Ui {
	class CompareWidget;
}

class GitBranchesModel;
class GitHistoryModel;

class QModelIndex;



/**
 * @brief The commit history comparison tab.
 */
class CompareWidget : public QWidget
{
	Q_OBJECT
	public:
		explicit CompareWidget(QWidget *parent = 0);
		~CompareWidget();

	public slots:
		void setRepository(Git::Repo *repo);

	private:
		void loadModels();
		void showCurrentBranch();
		void updateComparison();

	private slots:
		void on_branchAComboBox_currentIndexChanged(const QString&);
		void on_branchBComboBox_currentIndexChanged(const QString&);
		void on_historyAView_clicked(const QModelIndex &index);
		void on_historyBView_clicked(const QModelIndex &index);

	private:
		GitBranchesModel *m_branchesModel;
		GitHistoryModel *m_historyAModel;
		GitHistoryModel *m_historyBModel;
		Git::Id m_commitAId;
		Git::Id m_commitBId;
		Git::Repo *m_repo;
		Ui::CompareWidget *ui;
};

#endif // COMPAREWIDGET_H
