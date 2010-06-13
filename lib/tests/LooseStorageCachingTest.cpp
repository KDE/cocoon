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
			QString id = repo->commits()[0]->id();
			const char* pData1 = storage->rawDataFor(id).data();
			const char* pData2 = storage->rawDataFor(id).data();

			QVERIFY(pData1 != pData2);
		}

		void shouldCacheObjectDataBetweenQueries() {
			QString id = repo->commits()[0]->id();
			const char* pData1 = storage->objectDataFor(id).data();
			const char* pData2 = storage->objectDataFor(id).data();

			QVERIFY(pData1 == pData2);
		}

		void shouldCacheObjectsBetweenQueries() {
			QString id = repo->commits()[0]->id();
			Git::RawObject* pObject1 = storage->objectFor(id);
			Git::RawObject* pObject2 = storage->objectFor(id);

			QVERIFY(pObject1 == pObject2);
		}
};

QTEST_KDEMAIN_CORE(LooseStorageCachingTest);



#include "LooseStorageCachingTest.moc"
