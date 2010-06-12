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



class RawObjectTypeTest : public GitTestBase
{
	Q_OBJECT

	Git::RawObject *object;

	private slots:
		void init() {
			GitTestBase::init();

			object = new Git::RawObject("1234567", 0);
		}

		void cleanup() {
			delete object;

			GitTestBase::cleanup();
		}



		void testIsBlob() {
			object->m_type = Git::OBJ_BLOB;

			QVERIFY( object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsCommit() {
			object->m_type = Git::OBJ_COMMIT;

			QVERIFY(!object->isBlob());
			QVERIFY( object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsTag() {
			object->m_type = Git::OBJ_TAG;

			QVERIFY(!object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY( object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsTree() {
			object->m_type = Git::OBJ_TREE;

			QVERIFY(!object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY( object->isTree());
		}
};

QTEST_KDEMAIN_CORE(RawObjectTypeTest);

#include "RawObjectTypeTest.moc"
