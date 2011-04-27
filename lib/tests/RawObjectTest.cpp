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

#include "Git/Commit.h"
#include "Git/LooseStorage.h"
#include "Git/RawObject.h"
#include "Git/RawObject_p.h"



class RawObjectTest : public GitTestBase
{
	Q_OBJECT

	Git::RawObject *object;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			cloneFrom("RawObjectTestRepo");
		}

		void init() {
			GitTestBase::init();

			object = new Git::RawObject(repo->idFor("c56dada"), *repo);
		}

		void cleanup() {
			delete object;
			GitTestBase::cleanup();
		}



		void shouldOnlyPopulateHeaderOnCreation() {
			QVERIFY(object->d->type != Git::OBJ_NONE);
			QVERIFY(object->d->dataSize >= 0);
		}

		void shouldPopulateFullyOnDataAccess() {
			QVERIFY(!object->data().isNull());
		}
};

QTEST_KDEMAIN_CORE(RawObjectTest)

#include "RawObjectTest.moc"
