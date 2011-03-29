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
 * @brief The dialog for cloning a repository and showing the progress.
 */

#ifndef CLONEREPOSITORYDIALOG_H
#define CLONEREPOSITORYDIALOG_H

#include <QDialog>

#include <QDir>

namespace Ui {
	class CloneRepositoryDialog;
}



/**
 * @brief The dialog for cloning a repository and showing the progress.
 */
class CloneRepositoryDialog : public QDialog
{
	Q_OBJECT

	public:
		CloneRepositoryDialog(QWidget *parent = 0);
		~CloneRepositoryDialog();

		QString repositoryPath() const;

	private:
		void startCloning();
		void enableClone();
		void enableFinish();

	private slots:
		void on_cleanLocalDirCheckBox_toggled(bool checked);
		void on_cloneUrlRequester_textChanged(const QString &text);
		void on_cloneButton_clicked();
		void on_localUrlRequester_textChanged(const QString &text);
		void on_stackedWidget_currentChanged(int index);
		void slotCloningFinished();
		void slotCloningProgress(QString progress);

	private:
		bool m_finished;
		QDir::Filters m_localDirFilters;
		Ui::CloneRepositoryDialog *ui;
};

#endif // CLONEREPOSITORYDIALOG_H
