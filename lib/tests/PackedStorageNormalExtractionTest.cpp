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



class PackedStorageNormalExtractionTest : public GitTestBase
{
	Q_OBJECT

	QString packName;
	Git::PackedStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("PackedStorageTestRepo");
		}

		void init() {
			GitTestBase::init();

			packName = "pack-6e6471a59ce1759c3fb91189036c587088492a8d";

			storage = new Git::PackedStorage(packName, *repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}



		void normalObjectOffestShouldBeCorrect() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			quint32 offset = storage->dataOffsetFor(id);

			QCOMPARE(offset, (unsigned)0x177);
		}

		void normalObjectTypeShouldBeCorrect() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			Git::ObjectType type = storage->objectTypeFor(id);

			QCOMPARE(type, Git::OBJ_COMMIT);
		}

		void normalObjectSizeShouldBeCorrect() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			int size = storage->objectSizeFor(id);

			QCOMPARE(size, 212);
		}

		void shouldExtractNormalObject() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			QByteArray data = storage->objectDataFor(id);

			QCOMPARE(QTest::toHexRepresentation(data, 12), QTest::toHexRepresentation("tree 5b36b1f", 12));
			QCOMPARE(data.size(), 212);
		}
};

QTEST_KDEMAIN_CORE(PackedStorageNormalExtractionTest);



#include "PackedStorageNormalExtractionTest.moc"
