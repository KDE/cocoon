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

#ifndef GITUNSTAGEDFILESMODEL_H
#define GITUNSTAGEDFILESMODEL_H

#include <QAbstractTableModel>

namespace Git {
	class Repo;
	class Status;
	class StatusFile;
}



class GitUnstagedFilesModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		explicit GitUnstagedFilesModel(const Git::Repo &repo, QObject *parent = 0);

		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		const QString& columnName(int column) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		const Git::StatusFile* mapToStatusFile(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;

	public slots:
		void reset();

	private:
		void loadFiles();

	private:
		QList<Git::StatusFile*> m_files;
		const Git::Repo &m_repo;
};

#endif // GITUNSTAGEDFILESMODEL_H
