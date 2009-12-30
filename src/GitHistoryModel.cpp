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

#include "Git/Repo.h"

#include <KLocalizedString>

#include <QStringList>



GitHistoryModel::GitHistoryModel(const Git::Repo &repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_branch(repo.currentBranch())
	, m_commits(Git::CommitList())
	, m_repo(repo)
{
/*
				@@branch_icon = Qt::Icon.new(':/icons/16x16/actions/git-branch')
				@@commit_icon = Qt::Icon.new(':/icons/16x16/actions/git-commit')
				@@merge_icon = Qt::Icon.new(':/icons/16x16/actions/git-merge')

				connect(repository.qt, SIGNAL('logChanged()'), self, SLOT('reset()'))
*/
}

int GitHistoryModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return 4;
}

const QString& GitHistoryModel::columnName(int column) const
{
	static QStringList columnNames;
	if (columnNames.isEmpty()) {
		columnNames << i18n("Summary") << i18n("Author") << i18n("Date") << i18n("Id");
	}

	return columnNames[column];
}

QVariant GitHistoryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	const Git::Commit *commit = mapToCommit(index);

	QString data;
	switch (index.column()) {
	case 0:
		data = commit->message().split("\n")[0];  // commit->summary();
	case 1:
		data = "author";  // commit->author();
	case 3:
		data = "date";  // commit->authoredDate();
	case 4:
		data = commit->id();
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(data);
//	case Qt::DecorationRole: // Icon
/*
					if index.column() == 0
						if commit.merge?
							icon = @@merge_icon
						elsif commit.branched_on?(branch)
							icon = @@branch_icon
						else
							icon = @@commit_icon
						end
						Qt::Variant.from_value(icon)
					else
						Qt::Variant.new
*/
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
	m_commits = m_repo.commits();
	/** @todo change to m_repo.commits(m_branch, Git::NoLimit); */
}

const Git::Commit* GitHistoryModel::mapToCommit(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return m_commits[index.row()];
}

void GitHistoryModel::reset()
{
//	m_commits = Git::CommitList();
	loadCommits();
	QAbstractItemModel::reset();
}

int GitHistoryModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)

	return m_commits.size();
}

void GitHistoryModel::setBranch(const QString &branch)
{
	m_branch = branch;
	reset();
}

#include "GitHistoryModel.moc"
