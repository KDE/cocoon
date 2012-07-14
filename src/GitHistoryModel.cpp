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

#include "GitHistoryModel.h"

#include <Git/Ref.h>
#include <Git/Repo.h>

#include <KIcon>
#include <KLocalizedString>

#include <QtCore/QStringList>



GitHistoryModel::GitHistoryModel(Git::Repo &repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_branch(repo.currentHead().name())
	, m_commits()
	, m_repo(repo)
{
	connect(&m_repo, SIGNAL(historyChanged()), this, SLOT(reset()));

	setBranch(m_branch);
}

int GitHistoryModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return 3;
}

const QString& GitHistoryModel::columnName(int column) const
{
	static QStringList columnNames;
	if (columnNames.isEmpty()) {
		columnNames << i18n("Date") << i18n("Author") << i18n("Summary");
	}

	return columnNames[column];
}

QVariant GitHistoryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	Git::Commit commit = mapToCommit(index);

	QString data;
	switch (index.column()) {
	case 0: // date
		data = commit.authoredAt().toString();
		break;
	case 1: // author
		data = commit.author();
		break;
	case 2: // summary
		data = commit.summary();
		break;
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(data);
	case Qt::DecorationRole: // Icon
		if (index.column() == 2) { // in summary column
			if (commit.isMerge()) {
				return QVariant(KIcon("git-merge"));
			} else if(commit.hasBranchedOn(QStringList() << m_branch)) {
				return QVariant(KIcon("git-branch"));
			} else {
				return QVariant(KIcon("git-commit"));
			}
		} else {
			return QVariant();
		}
	default:
		return QVariant();
	}
}

QVariant GitHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void GitHistoryModel::loadCommits()
{
	m_commits = m_repo.commits(m_branch);
	/** @todo change to m_repo.commits(m_branch, Git::NoLimit); */
}

Git::Commit GitHistoryModel::mapToCommit(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return Git::Commit();
	}

	return m_commits[index.row()];
}

void GitHistoryModel::reset()
{
	beginResetModel();
	loadCommits();
	endResetModel();
}

int GitHistoryModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_commits.size();
}

void GitHistoryModel::setBranch(const QString &branch)
{
	m_branch = branch;
	reset();
}

#include "GitHistoryModel.moc"
