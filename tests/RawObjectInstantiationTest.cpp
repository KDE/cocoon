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

#include "LooseStorage.h"
#include "RawObject.h"



class RawObjectInstantiationTest : public GitTestBase
{
	Q_OBJECT

	Git::RawObject *object;
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
			storage = new Git::LooseStorage(*repo);

			QString id = storage->allObjectsByType("commit")[0]->id();
			object = new Git::RawObject(id, *storage);
		}

		void cleanup() {
			delete object;
			delete storage;
			GitTestBase::cleanup();
		}



		void shouldCreateInstanceOfCommit() {
			QString id = storage->allObjectsByType("commit")[0]->id();

			Git::RawObject *obj = Git::RawObject::newInstance(id, *storage);

			Q_ASSERT(obj);
			QCOMPARE(obj->type(), QString("commit"));
			QCOMPARE(QString(obj->metaObject()->className()), QString("Git::Commit"));
		}
};

QTEST_KDEMAIN_CORE(RawObjectInstantiationTest);

#include "RawObjectInstantiationTest.moc"
