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

#ifndef COMMITWIDGET_H
#define COMMITWIDGET_H

#include <QWidget>

namespace Git {
	class Repo;
	class Status;
}

namespace Ui {
	class CommitWidget;
}



class CommitWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit CommitWidget(QWidget *parent = 0);
		~CommitWidget();

		void clear();
		void reload();

	public slots:
		void commit();
		void setRepository(Git::Repo *repo);

	private:
		bool error();
		Git::Status* repoStatus();

	private slots:
		void enableCommit();
		void on_commitButton_clicked();
		void on_Repo_indexChanged();

	private:
		QString m_errorMessage;
		Git::Repo *m_repo;
		Git::Status *m_status;
		Ui::CommitWidget *ui;
};

#endif // COMMITWIDGET_H
