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

#ifndef STAGEWIDGET_H
#define STAGEWIDGET_H

#include <QWidget>

namespace Git {
	class Repo;
}

namespace Ui {
	class StageWidget;
}

class GitFileStatusModel;

class QMenu;
class QModelIndex;
class QSortFilterProxyModel;



class StageWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit StageWidget(QWidget *parent = 0);
		~StageWidget();

	public slots:
		void commit();
		void reload();
		void setRepository(Git::Repo *repo);
		void stageFile();
		void unstageFile();

	private:
		void loadModels();
		void setupActions();

	private slots:
		void on_stagedChangesView_clicked(const QModelIndex &index);
		void on_stagedChangesView_customContextMenuRequested(const QPoint &pos);
		void on_stagedChangesView_doubleClicked(const QModelIndex &index);
		void on_unstagedChangesView_clicked(const QModelIndex &index);
		void on_unstagedChangesView_customContextMenuRequested(const QPoint &pos);
		void on_unstagedChangesView_doubleClicked(const QModelIndex &index);

	private:
		Git::Repo *m_repo;
		GitFileStatusModel    *m_stagedFilesModel;
		QSortFilterProxyModel *m_stagedFilesProxyModel;
		QMenu                 *m_stageWidgetContextMenu;
		GitFileStatusModel    *m_unstagedFilesModel;
		QSortFilterProxyModel *m_unstagedFilesProxyModel;
		QMenu                 *m_unstageWidgetContextMenu;
		Ui::StageWidget *ui;
};

#endif // STAGEWIDGET_H
