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

#include "Git/Id.h"
#include "Git/Id_p.h"
#include "Git/ObjectStorage.h"



class IdTest : public GitTestBase
{
	Q_OBJECT

	Git::ObjectStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			cloneFrom("IdTestRepo");
		}

		void init() {
			GitTestBase::init();

			storage = repo->storages()[0];
		}

		void cleanup() {
			storage = 0;

			GitTestBase::cleanup();
		}



		void shouldHaveDefaultConstructor() {
			Git::Id id = Git::Id();

			QCOMPARE(id.d->sha1, QString());
			QVERIFY(!id.d->storage);
		}

		void shouldInstantiateWithRepo() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo);

			QCOMPARE(id.d->sha1, QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QVERIFY(id.d->storage);
		}

		void shouldInstantiateWithStorage() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QCOMPARE(id.d->sha1, QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QCOMPARE(id.d->storage, storage);
		}

		void shouldBeCopyable() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);
			Git::Id copyId = id;

			QCOMPARE(copyId.d->sha1, QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
			QCOMPARE(copyId.d->storage, storage);
		}

		void copyShouldShareInternalData() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);
			Git::Id copyId = id;

			QCOMPARE(id.d.constData(), copyId.d.constData());
		}

		void constructedWithDefaultCtorShouldNotBeValid() {
			Git::Id id = Git::Id();

			QVERIFY(!id.exists());
		}

		void existingIdInRepoShouldBeValid() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *repo);

			QVERIFY( id.exists());
			QCOMPARE(id.d->storage, storage);
		}

		void existingIdInStorageShouldBeValid() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QVERIFY( id.exists());
			QCOMPARE(id.d->storage, storage);
		}

		void nonExistingIdInRepoShouldNotBeValid() {
			Git::Id id("1234567", *repo);

			QVERIFY(!id.exists());
			QVERIFY(!id.d->storage);
			QCOMPARE(id.d->sha1, QString());
		}

		void nonExistingIdInStorageShouldNonBeValid() {
			Git::Id id("1234567", *storage);

			QVERIFY(!id.exists());
			QVERIFY( id.d->storage);
			QCOMPARE(id.d->sha1, QString());
		}

		void shouldDetermineExistanceCorrectly() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QVERIFY(id.exists());
			QVERIFY(id.storage().contains(id.toSha1String()));
		}

		void shouldFindActualIdInRepo() {
			Git::Id id("d97d4ab", *repo);

			QCOMPARE(id.toSha1String(), QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void shouldFindActualIdInStorage() {
			Git::Id id("d97d4ab", *storage);

			QCOMPARE(id.toSha1String(), QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void shouldProduceCorrectShortId() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QCOMPARE(id.toShortSha1String(), QLatin1String("d97d4ab"));
		}

		void toStringShouldProduceFullId() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QCOMPARE(id.toString(), QLatin1String("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void shouldProduceCorrectBinaryId() {
			Git::Id id("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7", *storage);

			QCOMPARE(id.toBinarySha1(), QByteArray::fromHex("d97d4abbeb30f34fe85a2075b30103a3dcaf7fc7"));
		}

		void comparisonWithEqualIdFromSameStorageShouldBeCorrect() {
			Git::Id      id("d97d4ab", *storage);
			Git::Id otherId("d97d4ab", *storage);

			QVERIFY( (id == otherId));
			QVERIFY(!(id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void comparisonWithEqualIdFromDifferentStorageShouldBeCorrect() {
			Git::Repo otherRepo(repo->workingDir());

			Git::Id      id("d97d4ab", *storage);
			Git::Id otherId("d97d4ab", *otherRepo.storages()[0]);

			QVERIFY( (id == otherId));
			QVERIFY(!(id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void comparisonWithLargerIdShouldBeCorrect() {
			Git::Id      id("d97d4ab", *storage);
			Git::Id otherId("e2994c5", *storage);

			QVERIFY(!(id == otherId));
			QVERIFY( (id != otherId));
			QVERIFY( (id <  otherId));
		}

		void comparisonWithSmallerIdShouldBeCorrect() {
			Git::Id      id("d97d4ab", *storage);
			Git::Id otherId("4bd4b7b", *storage);

			QVERIFY(!(id == otherId));
			QVERIFY( (id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void shouldProduceCorrectHash() {
			Git::Id id("d97d4ab", *storage);

			QCOMPARE(qHash(id), (unsigned)0xd97d4abb);
		}
};

QTEST_KDEMAIN_CORE(IdTest);

#include "IdTest.moc"
