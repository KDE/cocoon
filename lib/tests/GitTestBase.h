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

#ifndef GITTESTBASE_H
#define GITTESTBASE_H

#include <QObject>

#include "Git/Repo.h"
#include "Git/Status.h"

#include <qtest_kde.h>

#include <QProcess>



class GitTestBase : public QObject
{
	Q_OBJECT

	protected:
		/**
		 * @brief Copies a prepared test reporitory to #workingDir.
		 *
		 * You can set the COCOON_GIT_TEST_REPOS_PATH environment variable to point to the directory with the test repos.
		 * Call this once before all the test functions or before each test function depending on whether the repo gets modified in a test.
		 *
		 * @see initTestCase(), init()
		 */
		void cloneFrom(const QString &name);
		QString newTempDirName();
		void deleteFile(const QString &file);
		QStringList gitBasicOpts();
		QString pathTo(const QString &file);
		void writeToFile(const QString &filePath, const QByteArray &content);

	protected slots:
		/**
		 * @brief Will be called before the first testfunction is executed
		 *
		 * This will reset #repo and set #workingDir to a new random temporary directory.
		 * It will not copy the repository, you will have to do that on your own.
		 *
		 * @see cloneFrom()
		 */
		void initTestCase();

		/**
		 * @brief Will be called after the last testfunction was executed
		 *
		 * It will delete the temporary directory.
		 */
		void cleanupTestCase();

		/**
		 * @brief Will be called before each testfunction is executed
		 *
		 * This will create a new Git::Repo object accessible through #repo.
		 */
		void init();

		/**
		 * @brief Will be called after every testfunction
		 *
		 * This will delete the Git::Repo in #repo and reset it.
		 */
		void cleanup();

	protected:
		Git::Repo *repo;
		QString clonedFrom;
		QString workingDir;
};

#endif // GITTESTBASE_H
