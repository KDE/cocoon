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
#include "Git/RawObject.h"



class RawObjectInstantiationTest : public GitTestBase
{
	Q_OBJECT

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RawObjectInstantiationTestRepo");
		}



		void shouldCreateInstanceOfBlob() {
			Git::RawObject *obj = Git::RawObject::newInstance(repo->idFor("86e041d"));

			Q_ASSERT(obj);
			QCOMPARE(obj->type(), Git::OBJ_BLOB);
			QCOMPARE(QString(obj->metaObject()->className()), QString("Git::Blob"));
		}

		void shouldCreateInstanceOfCommit() {
			Git::RawObject *obj = Git::RawObject::newInstance(repo->idFor("c56dada"));

			Q_ASSERT(obj);
			QCOMPARE(obj->type(), Git::OBJ_COMMIT);
			QCOMPARE(QString(obj->metaObject()->className()), QString("Git::Commit"));
		}

		void shouldCreateInstanceOfTree() {
			Git::RawObject *obj = Git::RawObject::newInstance(repo->idFor("5b36b1f"));

			Q_ASSERT(obj);
			QCOMPARE(obj->type(), Git::OBJ_TREE);
			QCOMPARE(QString(obj->metaObject()->className()), QString("Git::Tree"));
		}
};

QTEST_KDEMAIN_CORE(RawObjectInstantiationTest)

#include "RawObjectInstantiationTest.moc"
