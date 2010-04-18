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

			packName = "pack-ffd4ffdf47bddc07d2675702e50ac3b3b6df20bb";

			storage = new Git::PackedStorage(packName, *repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
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
			QCOMPARE(storage->allIds().size(), 3);
		}

		void shouldHaveCorrectObjectsInPack() {
			Q_ASSERT(storage->allIds()[0] == "5b36b1f1641c26c8bee07c40e2577be81a22c73d"); // tree
			Q_ASSERT(storage->allIds()[1] == "86e041dad66a19b9518b83b78865015f62662f75"); // blob
			Q_ASSERT(storage->allIds()[2] == "b7566b7883e0dd74baba8cb194ed5dacaed5bb62"); // commit
		}

		void shouldOnlyExtractHeader() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			QString rawHeader = storage->rawHeaderFor(id);

			QCOMPARE(rawHeader, QString("commit 212"));
		}

		void shouldExtractObject() {
			QString id = "b7566b7883e0dd74baba8cb194ed5dacaed5bb62";
			QByteArray data = storage->rawDataFor(id);

			QCOMPARE(QTest::toHexRepresentation(data, 23), QTest::toHexRepresentation("commit 212\0tree 5b36b1f", 23));
			QCOMPARE(data.size(), QString("commit 212").length() + 1 + 212);
		}

		void shouldFindObjectByFullId() {
			QCOMPARE(storage->rawObjectFor("b7566b7883e0dd74baba8cb194ed5dacaed5bb62")->id(), QLatin1String("b7566b7883e0dd74baba8cb194ed5dacaed5bb62"));
		}

		void shouldFindObjectByShordId() {
			QCOMPARE(storage->rawObjectFor("b7566b7")->id(), QLatin1String("b7566b7883e0dd74baba8cb194ed5dacaed5bb62"));
		}
};

QTEST_KDEMAIN_CORE(PackedStorageTest);



#include "PackedStorageTest.moc"
