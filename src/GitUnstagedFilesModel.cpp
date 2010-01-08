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

#include "GitUnstagedFilesModel.h"

#include "Git/Repo.h"
#include "Git/Status.h"

#include <KIcon>
#include <KLocalizedString>



GitUnstagedFilesModel::GitUnstagedFilesModel(const Git::Repo &repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_repo(repo)
{
	loadFiles();
}

int GitUnstagedFilesModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return 1;
}

const QString& GitUnstagedFilesModel::columnName(int column) const
{
	static QStringList columnNames;
	if (columnNames.isEmpty()) {
		columnNames << i18n("Unstaged Files");
	}

	return columnNames[column];
}

QVariant GitUnstagedFilesModel::data(const QModelIndex &index, int role) const
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

QVariant GitUnstagedFilesModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void GitUnstagedFilesModel::loadFiles()
{
	m_files = m_repo.status()->unstagedFiles();
}

const Git::StatusFile* GitUnstagedFilesModel::mapToStatusFile(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return m_files[index.row()];
}


void GitUnstagedFilesModel::reset()
{
	loadFiles();
	QAbstractItemModel::reset();
}

int GitUnstagedFilesModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_files.size();
}

#include "GitUnstagedFilesModel.moc"
