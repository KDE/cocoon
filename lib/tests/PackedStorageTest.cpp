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



class PackedStorageTest : public GitTestBase
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



		void shouldFindCorrectNumberOfPacks() {
			QCOMPARE(Git::PackedStorage::allNamesIn(*repo).size(), 1);
		}

		void shouldFindCorrectPacks() {
			QCOMPARE(Git::PackedStorage::allNamesIn(*repo)[0], packName);
		}

		void shouldHaveIndexFile() {
			QVERIFY(!storage->d->indexFile.fileName().isEmpty());
		}

		void shouldHaveCorrectIndexFile() {
			QCOMPARE(storage->d->indexFile.fileName(), QString("%1/objects/pack/%2.idx").arg(repo->gitDir()).arg(packName));
			QVERIFY(storage->d->indexFile.exists());
		}

		void shouldHavePackFile() {
			QVERIFY(!storage->d->packFile.fileName().isEmpty());
		}

		void shouldHaveCorrectPackFile() {
			QCOMPARE(storage->d->packFile.fileName(), QString("%1/objects/pack/%2.pack").arg(repo->gitDir()).arg(packName));
			QVERIFY(storage->d->packFile.exists());
		}

		void shouldHaveCorrectIndexVersion() {
			QCOMPARE(storage->d->indexVersion, (uint)2);
		}

		void shouldHaveCorrectNumberOfObjectsInPack() {
			QCOMPARE(storage->allIds().size(), 9);
		}

		void shouldHaveCorrectObjectsInPack() {
			QList<Git::Id> ids = storage->allIds();

			QCOMPARE(ids[0].toSha1String(), QLatin1String("4d1a5561d058e0b30e7752d4bebde44b6ac37faa")); // lorem_ipsum.txt in 2st commit
			QCOMPARE(ids[1].toSha1String(), QLatin1String("52076e870c962a1aa44a722bdcad8b75c04c9de1")); // tree for 3rd commit
			QCOMPARE(ids[2].toSha1String(), QLatin1String("5b36b1f1641c26c8bee07c40e2577be81a22c73d")); // tree for 1st commit
			QCOMPARE(ids[3].toSha1String(), QLatin1String("86e041dad66a19b9518b83b78865015f62662f75")); // some_file.txt
			QCOMPARE(ids[4].toSha1String(), QLatin1String("978dd50f4a265b83c793158a292ab83db82ded94")); // lorem_ipsum.txt in 3rd commit
			QCOMPARE(ids[5].toSha1String(), QLatin1String("a909f28454efc3fedce30447f997a3eeab6e3330")); // 3rd commit
			QCOMPARE(ids[6].toSha1String(), QLatin1String("b08e418477a380049d998de4815133ea5fb30b3a")); // 2nd commit
			QCOMPARE(ids[7].toSha1String(), QLatin1String("b7566b7883e0dd74baba8cb194ed5dacaed5bb62")); // 1st commit
			QCOMPARE(ids[8].toSha1String(), QLatin1String("f760db3a96a9392a765d3c29e305060368afceac")); // tree for 2nd commit
		}

		void shouldFindObjectByFullId() {
			QCOMPARE(storage->objectFor(Git::Id("b7566b7883e0dd74baba8cb194ed5dacaed5bb62", *storage)).id().toSha1String(), QLatin1String("b7566b7883e0dd74baba8cb194ed5dacaed5bb62"));
		}

		void shouldFindObjectByShordId() {
			QCOMPARE(storage->objectFor(Git::Id("b7566b7", *storage)).id().toSha1String(), QLatin1String("b7566b7883e0dd74baba8cb194ed5dacaed5bb62"));
		}

		void shoudlCacheIdsBetweenQueries() {
			const Git::Id *pId1 = &storage->allIds()[0];
			const Git::Id *pId2 = &storage->allIds()[0];

			QVERIFY(pId1 == pId2);
		}
};

QTEST_KDEMAIN_CORE(PackedStorageTest);



#include "PackedStorageTest.moc"
