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

#ifndef GITHISTORYMODEL_H
#define GITHISTORYMODEL_H

#include <QtCore/QAbstractTableModel>

namespace LibQGit2 {
	class QGitCommit;
	class QGitRepository;
	class QGitRevWalk;
}
using namespace LibQGit2;
#include <QGit2/QGitRevWalk>



class GitHistoryModel : public QAbstractTableModel
{
	Q_OBJECT

	public:
		explicit GitHistoryModel(QGitRepository &repo, QObject *parent = 0);

		const QString& branch();
		bool canFetchMore(const QModelIndex &parent) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		const QString& columnName(int column) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		void fetchMore(const QModelIndex &parent);
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		QGitCommit mapToCommit(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;

	public slots:
		void setBranch(const QString &branch);

	protected slots:
		void reset();

	private:
		bool hasBranchedOn(const QStringList &refs, const QGitCommit &commit) const;

	private:
		QString m_branch;
		QList<QGitOId> m_commits;
		bool m_loadingFinished;
		QGitRepository &m_repo;
		QGitRevWalk m_revWalk;
};

#endif // GITHISTORYMODEL_H
