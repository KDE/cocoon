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

#ifndef COMPAREWIDGET_H
#define COMPAREWIDGET_H

#include <QWidget>

namespace Git {
	class Commit;
	class Repo;
}

namespace Ui {
	class CompareWidget;
}

class GitBranchesModel;
class GitHistoryModel;

class QModelIndex;



class CompareWidget : public QWidget
{
	Q_OBJECT
	public:
		explicit CompareWidget(QWidget *parent = 0);
		~CompareWidget();

	public slots:
		void setRepository(const Git::Repo *repo);

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
		const Git::Commit *m_commitA;
		const Git::Commit *m_commitB;
		const Git::Repo *m_repo;
		Ui::CompareWidget *ui;
};

#endif // COMPAREWIDGET_H
