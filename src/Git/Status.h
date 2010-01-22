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



class DeletedFileStatusTest;
class NewFileStatusTest;
class NewlyAddedFileStatusTest;
class ModifiedAddedFileStatusTest;
class ModifiedFileStatusTest;
class RemodifiedFileStatusTest;
class RemovedFileStatusTest;
class UpdatedFileStatusTest;

namespace Git {

class Repo;
class Status;



class StatusFile : public QObject
{
	Q_OBJECT

	friend class Status;

	public:
		StatusFile(const Repo *parent);

	enum StatusFlag {
		None      = 0x00,
		Staged    = 0x01,
		Unstaged  = 0x02,
		Untracked = 0x04,
		Added     = 0x08,
		Modified  = 0x10,
		Deleted   = 0x20
	};
	Q_DECLARE_FLAGS(Status, StatusFlag);


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
		Status status() const;

	private:
		QString m_idIndex;
		QString m_idRepo;
		QString m_modeIndex;
		QString m_modeRepo;
		QString m_path;
		const Repo *m_repo;
		bool m_staged;
		Status m_status;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(StatusFile::Status)



class Status : public QObject
{
	Q_OBJECT

	public:
		explicit Status(const Repo *repo);

		QList<StatusFile*> files() const;
		QList<StatusFile*> filesByStatus(StatusFile::Status fileStatus) const;
		QList<StatusFile*> forFile(const QString &file) const;
		QList<StatusFile*> stagedFiles() const;
		QList<StatusFile*> unstagedFiles() const;

	private:
		void constuctStatus();
		void addFile(StatusFile *file);
		/** Compares the index and the working directory */
		QList<StatusFile*> diffFiles() const;
		/** Compares the index and the repository */
		QList<StatusFile*> diffIndex(const QString &treeish) const;
		QList<StatusFile*> ignoredFiles() const;
		QList<StatusFile*> lsFiles() const;
		StatusFile::Status statusFromString(const QString &status) const;
		QString unescapeFileName(const QString &escapedName) const;
		QList<StatusFile*> untrackedFiles() const;

	private:
		QList<StatusFile*> m_files;
		const Repo *m_repo;
		QHash<QString, QList<StatusFile*> > m_status;

		friend class ::DeletedFileStatusTest;
		friend class ::NewFileStatusTest;
		friend class ::NewlyAddedFileStatusTest;
		friend class ::ModifiedAddedFileStatusTest;
		friend class ::ModifiedFileStatusTest;
		friend class ::RemodifiedFileStatusTest;
		friend class ::RemovedFileStatusTest;
		friend class ::UpdatedFileStatusTest;
};

}

#endif // STATUS_H
