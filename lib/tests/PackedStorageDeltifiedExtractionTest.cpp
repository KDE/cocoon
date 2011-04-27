/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2010  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#include "Git/PackedStorage.h"



class PackedStorageDeltifiedExtractionTest : public GitTestBase
{
	Q_OBJECT

	QString packName;
	Git::PackedStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("PackedStorageDeltifiedExtractionTestRepo");
		}

		void init() {
			GitTestBase::init();

			packName = "pack-f8a3b0b5e0629c6ad65fa669f9af2f0bc9db0ffd";

			storage = new Git::PackedStorage(packName, *repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}



		void deltifiedObjectOffsetShouldBeCorrect() {
			Git::Id id = repo->idFor("7096645");
			quint32 offset = storage->dataOffsetFor(id);

			QCOMPARE(offset, (unsigned)0x1cd);
		}


		void deltifiedObjectTypeShouldBeCorrect() {
			Git::Id id = repo->idFor("7096645");
			Git::ObjectType type = storage->objectTypeFor(id);

			QCOMPARE(type, Git::OBJ_BLOB);
		}

		void deltifiedObjectSizeShouldBeCorrect() {
			Git::Id id = repo->idFor("7096645");
			int size = storage->objectSizeFor(id);

			QCOMPARE(size, 182);
		}

		void shouldExtractDeltifiedObject() {
			Git::Id id = repo->idFor("7096645");
			QByteArray data = storage->objectDataFor(id);

			QCOMPARE(QTest::toHexRepresentation(data, 27), QTest::toHexRepresentation("#!/bin/env/ruby\n\ndef do_foo", 27));
			QCOMPARE(QTest::toHexRepresentation(data.right(20), 20), QTest::toHexRepresentation("enough!\"\nend\n\ndo_foo", 20));
			QCOMPARE(data.size(), 182);
		}
};

QTEST_KDEMAIN_CORE(PackedStorageDeltifiedExtractionTest)



#include "PackedStorageDeltifiedExtractionTest.moc"
