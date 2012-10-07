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

#include "GitHistoryModel.h"

#include "CommitHelper.h"
using namespace Cocoon;

#include <QGit2/QGit2>

#include <KDebug>
#include <KIcon>
#include <KLocalizedString>

#include <QtCore/QStringList>



GitHistoryModel::GitHistoryModel(QGitRepository &repo, QObject *parent)
	: QAbstractTableModel(parent)
	, m_branch()
	, m_commits()
	, m_loadingFinished(false)
	, m_repo(repo)
	, m_revWalk(repo)
{
	m_revWalk.setSorting(QGitRevWalk::Topological | QGitRevWalk::Time);

	setBranch(m_branch);
}

bool GitHistoryModel::canFetchMore(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return !m_loadingFinished;
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

	QGitCommit commit = mapToCommit(index);

	QString data;
	switch (index.column()) {
	case 0: // date
		data = commit.author().when().toString();
		break;
	case 1: // author
		data = commit.author().name();
		break;
	case 2: // summary
		data = commit.shortMessage(commit.message().indexOf("\n"));
		break;
	}

	switch (role) {
	case Qt::DisplayRole:
		return QVariant(data);
	case Qt::DecorationRole: // Icon
		if (index.column() == 2) { // in summary column
			if (CommitHelper::isMerge(commit)) {
				return QVariant(KIcon("git-merge"));
			} else if(hasBranchedOn(QStringList() << m_branch, commit)) {
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

void GitHistoryModel::fetchMore(const QModelIndex &parent)
{
	static const int BATCH_SIZE = 100;
	QVector<QGitOId> newCommits = QVector<QGitOId>();
	newCommits.reserve(BATCH_SIZE);

	do {
		QGitOId newId = m_revWalk.next();
		if (!newId.isValid()) {
			m_loadingFinished = true;
		} else {
			newCommits << newId;
		}
	} while (newCommits.size() < BATCH_SIZE && !m_loadingFinished);

	beginInsertRows(parent, m_commits.size(), m_commits.size()+newCommits.size()-1);
	m_commits << newCommits.toList();
	endInsertRows();
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

bool GitHistoryModel::hasBranchedOn(const QStringList &refs, const QGitCommit &commit) const
{
	Q_UNUSED(refs);
	Q_UNUSED(commit);
	// FIXME: implement
	return false;
}

QGitCommit GitHistoryModel::mapToCommit(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return QGitCommit();
	}

	return m_repo.lookupCommit(m_commits[index.row()]);
}

void GitHistoryModel::reset()
{
	beginResetModel();
	m_commits.clear();
	m_revWalk.reset();
	int error = m_revWalk.pushRef(m_branch);
	if (error) {
		kDebug() << error << giterr_last();
	}
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
