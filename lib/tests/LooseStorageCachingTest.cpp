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
#include "Git/LooseStorage_p.h"



class LooseStorageCachingTest : public GitTestBase
{
	Q_OBJECT

	Git::LooseStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("LooseStorageCachingTestRepo");
		}

		void init() {
			GitTestBase::init();
			storage = new Git::LooseStorage(*repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}



		void shouldNotCacheRawDataBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			const char* pData1 = storage->rawDataFor(id).constData();
			const char* pData2 = storage->rawDataFor(id).constData();

			// brittle
			QVERIFY(pData1 != pData2);
		}

		void shouldCacheObjectDataBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			const char* pData1 = storage->objectDataFor(id).constData();
			const char* pData2 = storage->objectDataFor(id).constData();

			QVERIFY(pData1 == pData2);
		}

		void shouldCacheObjectsBetweenQueries() {
			Git::Id id = repo->commits()[0].id();
			Git::RawObject &pObject1 = storage->objectFor(id);
			Git::RawObject &pObject2 = storage->objectFor(id);

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
			Git::Id id = repo->idFor("c56dada");
			Git::RawObject *pOldObject = &storage->objectFor(id);
			storage->reset();
			Git::RawObject *pNewObject = &storage->objectFor(id);

			// brittle
			QVERIFY(pNewObject != pOldObject);
		}

		void resetShouldClearObjectData() {
			Git::Id id = repo->idFor("c56dada");
			const char* oldData = storage->objectDataFor(id).constData();
			storage->reset();
			const char* newData = storage->objectDataFor(id).constData();

			// brittle
			QVERIFY(newData != oldData);
		}

		void resetShouldClearObjectSizes() {
			Git::Id id = repo->idFor("c56dada");
			storage->objectSizeFor(id); // load data
			storage->reset();

			QVERIFY(storage->d->objectSizes.isEmpty());
		}

		void resetShouldClearObjectTypes() {
			Git::Id id = repo->idFor("c56dada");
			storage->objectTypeFor(id); // load data
			storage->reset();

			QVERIFY(storage->d->objectTypes.isEmpty());
		}

		void repoHistoryChangeShouldTriggerReset() {

		}
};

QTEST_KDEMAIN_CORE(LooseStorageCachingTest)



#include "LooseStorageCachingTest.moc"
