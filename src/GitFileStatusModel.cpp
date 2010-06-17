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

#include "GitFileStatusModel.h"

#include "Git/Repo.h"
#include "Git/Status.h"

#include <KIcon>
#include <KLocalizedString>



GitFileStatusModel::GitFileStatusModel(Git::StatusFile::Status fileStatus, Git::Repo *repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_fileStatus(fileStatus)
	, m_repo(repo)
{
	connect(m_repo, SIGNAL(indexChanged()), this, SLOT(reset()));

	loadFiles();
}

int GitFileStatusModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return 1;
}

const QString& GitFileStatusModel::columnName(int column) const
{
	static QStringList columnNames;
	if (columnNames.isEmpty()) {
		columnNames << i18n("Unstaged Files");
	}

	return columnNames[column];
}

QVariant GitFileStatusModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	const Git::StatusFile *file = mapToStatusFile(index);

	QString data;
	switch (index.column()) {
	case 0:
		data = file->path();
		break;
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(data);
	case Qt::DecorationRole: // Icon
		if (index.column() == 0) { // in first column
			if (file->isAdded()) {
				return QVariant(KIcon("git-file-added"));
			} else if(file->isDeleted()) {
				return QVariant(KIcon("git-file-deleted"));
			} else if(file->isModified()) {
				return QVariant(KIcon("git-file-modified"));
			} else {
				return QVariant(KIcon("git-file-untracked"));
			}
		} else {
			return QVariant();
		}
	default:
		return QVariant();
	}
}

Git::StatusFile::Status GitFileStatusModel::fileStatus() const
{
	return m_fileStatus;
}

QVariant GitFileStatusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section > columnCount() || orientation != Qt::Horizontal) {
		return QVariant();
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(columnName(section));
	default:
		return QVariant();
	}
}

void GitFileStatusModel::loadFiles()
{
	m_files = m_repo->status()->filesByStatus(m_fileStatus);
}

QModelIndex GitFileStatusModel::mapToIndex(const Git::StatusFile &file) const
{
	for (int i=0; i < m_files.size(); ++i) {
		if (m_files[i]->path() == file.path()) {
			return index(i, 0);
		}
	}

	return QModelIndex();
}

const Git::StatusFile* GitFileStatusModel::mapToStatusFile(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return m_files[index.row()];
}


void GitFileStatusModel::reset()
{
	loadFiles();
	QAbstractItemModel::reset();
}

int GitFileStatusModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_files.size();
}

#include "GitFileStatusModel.moc"
