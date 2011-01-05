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
#include "Git/PackedStorage_p.h"



class PackedStorageCachingTest : public GitTestBase
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



		void shouldCachePackObjectsBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			Git::PackedStorageObject *pData1 = storage->packObjectFor(id);
			Git::PackedStorageObject *pData2 = storage->packObjectFor(id);

			QVERIFY(pData1 == pData2);
		}

		void shouldCacheObjectDataBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			const char* pData1 = storage->objectDataFor(id).constData();
			const char* pData2 = storage->objectDataFor(id).constData();

			QVERIFY(pData1 == pData2);
		}

		void shouldCacheObjectsBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			Git::RawObject *pObject1 = &storage->objectFor(id);
			Git::RawObject *pObject2 = &storage->objectFor(id);

			QVERIFY(pObject1 == pObject2);
		}

		void shoudlCacheIdsBetweenQueries() {
			const Git::Id *pId1 = &storage->allIds()[0];
			const Git::Id *pId2 = &storage->allIds()[0];

			QVERIFY(pId1 == pId2);
		}

		void resetShouldClearIds() {
			const Git::Id *pOldId = &storage->allIds()[0];
			storage->reset();
			const Git::Id *pNewId = &storage->allIds()[0];

			QVERIFY(pOldId != pNewId);
		}

		void resetShouldClearObjects() {
			Git::Id id = Git::Id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);
			Git::RawObject *pOldObject = &storage->objectFor(id);
			storage->reset();
			Git::RawObject *pNewObject = &storage->objectFor(id);

			QVERIFY(pNewObject != pOldObject);
		}

		void resetShouldPackObjectSizes() {
			Git::Id id = Git::Id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);
			storage->packObjectFor(id); // load data
			storage->reset();

			QVERIFY(storage->d->packObjects.isEmpty());
		}

		void resetShouldReloadIndexDataOffsets() {
			storage->reset();

			QVERIFY(!storage->d->indexDataOffsets.isEmpty());
		}
};

QTEST_KDEMAIN_CORE(PackedStorageCachingTest);



#include "PackedStorageCachingTest.moc"
