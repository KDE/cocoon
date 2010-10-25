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

#ifndef COMITINFOWIDGET_H
#define COMITINFOWIDGET_H

#include <QWidget>

#include "Git/Id.h"

namespace Git {
	class Commit;
}

namespace Ui {
	class CommitInfoWidget;
}



class CommitInfoWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit CommitInfoWidget(QWidget *parent = 0);
		~CommitInfoWidget();

		void setCommit(Git::Commit *commit);

	public slots:
		void clear();
		void updateView();

	private:
		Git::Id m_commitId;
		Ui::CommitInfoWidget *ui;
};

#endif // COMITINFOWIDGET_H
