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

#ifndef STATUS_H
#define STATUS_H

#include <QObject>

#include <QHash>
#include <QList>



namespace Git {

class Repo;
class Status;



class StatusFile : public QObject
{
	Q_OBJECT

	friend class Status;

	public:
		StatusFile(const Repo *parent);

		const QByteArray blob(QString type = QString()) const;
		bool changesStaged() const;
		bool changesUnstaged() const;
		const QString diff() const;
		const QString& idIndex() const;
		const QString& idRepo() const;
		bool isAdded() const;
		bool isDeleted() const;
		bool isModified() const;
		bool isStaged() const;
		bool isUntracked() const;
		bool hasChanged() const;
		void merge(const StatusFile &file);
		const QString& modeIndex() const;
		const QString& modeRepo() const;
		const QString& path() const;
		const QString& status() const;

	private:
		QString m_idIndex;
		QString m_idRepo;
		QString m_modeIndex;
		QString m_modeRepo;
		QString m_path;
		const Repo *m_repo;
		bool m_staged;
		QString m_status;
};



class Status : public QObject
{
	Q_OBJECT

	public:
		explicit Status(const Repo *repo);

		QList<StatusFile*> files() const;
		QList<StatusFile*> forFile(const QString &file) const;
		QList<StatusFile*> stagedFiles() const;
		QList<StatusFile*> unstagedFiles() const;

	private:
		void constuctStatus();
		void addFile(const QString &file, QHash<QString, QString> data);
		/** Compares the index and the working directory */
		QHash<QString, QHash<QString, QString> > diffFiles() const;
		/** Compares the index and the repository */
		QHash<QString, QHash<QString, QString> > diffIndex(const QString &treeish) const;
		QStringList ignoredFiles() const;
		QHash<QString, QHash<QString, QString> > lsFiles() const;
		QString unescapeFileName(const QString &escapedName) const;
		QStringList untrackedFiles() const;

	private:
		QList<StatusFile*> m_files;
		const Repo *m_repo;
		QHash<QString, QList<StatusFile*> > m_status;
};

}

#endif // STATUS_H
