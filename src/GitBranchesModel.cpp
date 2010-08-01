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

#include "GitBranchesModel.h"

#include <Git/Ref.h>
#include <Git/Repo.h>

#include <KLocalizedString>



GitBranchesModel::GitBranchesModel(Git::Repo &repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_repo(repo)
{
	connect(&m_repo, SIGNAL(historyChanged()), this, SLOT(reset()));

	loadBranches();
}

int GitBranchesModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return 1;
}

const QString& GitBranchesModel::columnName(int column) const
{
	static QStringList columnNames;
	if (columnNames.isEmpty()) {
		columnNames << i18n("Name");
	}

	return columnNames[column];
}

QVariant GitBranchesModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	QString data;
	switch (index.column()) {
	case 0:
		data = m_branches[index.row()]->name();
		break;
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(data);
	default:
		return QVariant();
	}
}

QVariant GitBranchesModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void GitBranchesModel::loadBranches()
{
	m_branches = m_repo.heads();
}

void GitBranchesModel::reset()
{
	loadBranches();
	QAbstractItemModel::reset();
}

int GitBranchesModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_branches.size();
}
