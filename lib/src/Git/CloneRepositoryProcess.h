/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#ifndef CLONEREPOSITORYPROCESS_H
#define CLONEREPOSITORYPROCESS_H

#include <KProcess>
#include <kdemacros.h>



namespace Git {



class KDE_EXPORT CloneRepositoryProcess : public KProcess
{
	Q_OBJECT

	public:
		explicit CloneRepositoryProcess(const QString &fromRepo, const QString &toDirectory, QObject *parent = 0);

	signals:
		void cloningProgress(int);
		void cloningProgress(QString);
		void cloningFinished();

	private slots:
		void slotReadyReadStandardError();

	private:
		QString m_fromRepo;
		QString m_toDirectory;
};

}

#endif // CLONEREPOSITORYPROCESS_H
