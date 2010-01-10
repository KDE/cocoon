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

class GitStagedFilesModel;
class GitUnstagedFilesModel;

class QModelIndex;



class StageWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit StageWidget(QWidget *parent = 0);
		~StageWidget();

	public slots:
		void reload();
		void setRepository(Git::Repo *repo);

	private:
		void loadModels();
		void stageFile();
		void unstageFile();

	private slots:
		void on_stagedChangesView_doubleClicked(const QModelIndex &index);
		void on_unstagedChangesView_doubleClicked(const QModelIndex &index);

	private:
		Git::Repo *m_repo;
		GitStagedFilesModel *m_stagedFilesModel;
		GitUnstagedFilesModel *m_unstagedFilesModel;
		Ui::StageWidget *ui;
};

#endif // STAGEWIDGET_H
