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
 * @brief The dialog for opening a new repo.
 */

#ifndef OPENREPOSITORYDIALOG_H
#define OPENREPOSITORYDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
	class OpenRepositoryDialog;
}



/**
 * @brief The dialog for opening a new repo.
 */
class OpenRepositoryDialog : public QDialog
{
	Q_OBJECT
	public:
		OpenRepositoryDialog(QWidget *parent = 0);
		~OpenRepositoryDialog();

		virtual void accept();
		virtual void reject();
		QString selectedRepositoryPath() const;

	private:
		void addRepository(const QString &repoPath);
		void createActions();
		void loadRepositories();
		void saveRepositories();
		void selectRepository(const int row);
		void selectRepository(const QString &repoPath);

	private slots:
		void on_repositoriesListWidget_currentTextChanged(const QString&);
		void on_addButton_clicked();
		void on_cloneButton_clicked();
		void on_newButton_clicked();
		void on_removeButton_clicked();

	private:
		Ui::OpenRepositoryDialog *ui;
};

#endif // OPENREPOSITORYDIALOG_H
