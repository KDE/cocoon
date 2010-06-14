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

#include "Git/Commit.h"
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



		void deltifiedObjectTypeShouldBeCorrect() {
			QString id = "978dd50f4a265b83c793158a292ab83db82ded94";
			Git::ObjectType type = storage->objectTypeFor(id);

			QCOMPARE(type, Git::OBJ_BLOB);
		}

		void deltifiedObjectSizeShouldBeCorrect() {
			QString id = "978dd50f4a265b83c793158a292ab83db82ded94";
			int size = storage->objectSizeFor(id);

			QCOMPARE(size, 3107);
		}

		void shouldExtractDeltifiedObject() {
			QString id = "978dd50f4a265b83c793158a292ab83db82ded94";
			QByteArray data = storage->objectDataFor(id);

			QCOMPARE(QTest::toHexRepresentation(data, 27), QTest::toHexRepresentation("Lorem ipsum dolor sit amet,", 27));
			QCOMPARE(QTest::toHexRepresentation(data.right(28), 28), QTest::toHexRepresentation("dolore magna aliquyam erat.\n", 28));
			QCOMPARE(data.size(), 3107);
		}
};

QTEST_KDEMAIN_CORE(PackedStorageDeltifiedExtractionTest);



#include "PackedStorageDeltifiedExtractionTest.moc"
