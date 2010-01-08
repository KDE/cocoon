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



namespace Git {

class Repo;
class Status;



class StatusFile : public QObject
{
	Q_OBJECT

	friend class Status;

	public:
		StatusFile(Status *parent);

		bool changesStaged() const;
		bool changesUnstaged() const;
		bool isAdded() const;
		bool isDeleted() const;
		bool isModified() const;
		bool isStaged() const;
		bool isUntracked() const;
		bool hasChanged() const;
		const QString& path() const;

	private:
		QString m_idIndex;
		QString m_idRepo;
		QString m_modeIndex;
		QString m_modeRepo;
		QString m_path;
		bool m_staged;
		QString m_status;
};



class Status : public QObject
{
	Q_OBJECT

	public:
		explicit Status(const Repo *repo);

	private:
		const Repo *m_repo;
};

}

#endif // STATUS_H
