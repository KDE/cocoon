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

#include "GitTestBase.h"

#include "Git/LooseStorage.h"



class LooseStorageTest : public GitTestBase
{
	Q_OBJECT

	Git::LooseStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			writeToFile("some_file.txt", "foo\nbar\baz");

			QProcess::execute("git", gitBasicOpts() << "add" << "some_file.txt");
			QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added a file.");
		}

		void init() {
			GitTestBase::init();
			storage = new Git::LooseStorage(repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}

		void testSourceIsCorrect() {
			QString id = repo->commits()[0]->id();
			qDebug() << id;
			QString sourcePath = storage->sourceFor(id);

			QVERIFY(sourcePath == QString("%1/.git/objects/%2/%3").arg(repo->workingDir()).arg(id.left(2)).arg(id.mid(2)));
		}

		void testInflationIsWorking() {
			QString id = repo->commits()[0]->id();
			qDebug() << id;
			QByteArray data = storage->rawDataFor(id);

			QVERIFY(data.startsWith("commit 212"));
		}
};

QTEST_KDEMAIN_CORE(LooseStorageTest);

#include "LooseStorageTest.moc"
