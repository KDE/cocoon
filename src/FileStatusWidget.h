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
 * @brief A widget to show Git's status information of a single file.
 */

#ifndef FILESTATUSWIDGET_H
#define FILESTATUSWIDGET_H

#include <QWidget>

#include "Git/Status.h"

#include <KMimeType>

namespace Git {
	class Repo;
}

namespace Ui {
	class FileStatusWidget;
}



/**
 * @brief A widget to show Git's status information of a single file.
 */
class FileStatusWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit FileStatusWidget(QWidget *parent = 0);
		~FileStatusWidget();

		void setFile(const Git::StatusFile &file);

	public slots:
		void clear();
		void setRepository(Git::Repo *repo);

	private:
		QByteArray byteArray();
		bool isBinary();
		QByteArray fileData(QString type);
		QString fileType();
		KMimeType::Ptr mimeType();
		void showFileStatus();
		QString statusToIconName(Git::StatusFile::Status status);
		QString statusToString(Git::StatusFile::Status status);

	private:
		QByteArray m_byteArray;
		const Git::StatusFile *m_file;
		Git::Repo *m_repo;
		Ui::FileStatusWidget *ui;
};

#endif // FILESTATUSWIDGET_H
