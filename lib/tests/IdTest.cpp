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
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *repo);

			QCOMPARE(id.d->sha1, QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
			QVERIFY(id.d->storage);
		}

		void shouldInstantiateWithStorage() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QCOMPARE(id.d->sha1, QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
			QCOMPARE(id.d->storage, storage);
		}

		void shouldBeCopyable() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);
			Git::Id copyId = id;

			QCOMPARE(copyId.d->sha1, QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
			QCOMPARE(copyId.d->storage, storage);
		}

		void copyShouldShareInternalData() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);
			Git::Id copyId = id;

			QCOMPARE(id.d.constData(), copyId.d.constData());
		}

		void constructedWithDefaultCtorShouldNotBeValid() {
			Git::Id id = Git::Id();

			QVERIFY(!id.isValid());
			QVERIFY(!id.exists());
		}

		void existingIdInRepoShouldBeValid() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *repo);

			QVERIFY( id.isValid());
			QVERIFY( id.exists());
			QCOMPARE(id.d->storage, storage);
		}

		void existingIdInStorageShouldBeValid() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QVERIFY( id.isValid());
			QVERIFY( id.exists());
			QCOMPARE(id.d->storage, storage);
		}

		void nonExistingIdInRepoShouldNotBeValid() {
			Git::Id id("1234567", *repo);

			QVERIFY(!id.isValid());
			QVERIFY(!id.exists());
			QVERIFY(!id.d->storage);
			QCOMPARE(id.d->sha1, QString());
		}

		void nonExistingIdInStorageShouldNonBeValid() {
			Git::Id id("1234567", *storage);

			QVERIFY(!id.isValid());
			QVERIFY(!id.exists());
			QVERIFY( id.d->storage);
			QCOMPARE(id.d->sha1, QString());
		}

		void shouldDetermineExistanceCorrectly() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QVERIFY(id.exists());
			QVERIFY(id.storage().contains(id.toSha1String()));
		}

		void shouldFindActualIdInRepo() {
			Git::Id id("c56dada", *repo);

			QCOMPARE(id.toSha1String(), QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
		}

		void shouldFindActualIdInStorage() {
			Git::Id id("c56dada", *storage);

			QCOMPARE(id.toSha1String(), QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
		}

		void shouldProduceCorrectShortId() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QCOMPARE(id.toShortSha1String(), QLatin1String("c56dada"));
		}

		void toStringShouldProduceFullId() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QCOMPARE(id.toString(), QLatin1String("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
		}

		void shouldProduceCorrectBinaryId() {
			Git::Id id("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8", *storage);

			QCOMPARE(id.toBinarySha1(), QByteArray::fromHex("c56dada2cf4f67b35ed0019ddd4651a8c8a337e8"));
		}

		void comparisonWithEqualIdFromSameStorageShouldBeCorrect() {
			Git::Id      id("c56dada", *storage);
			Git::Id otherId("c56dada", *storage);

			QVERIFY( (id == otherId));
			QVERIFY(!(id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void comparisonWithEqualIdFromDifferentStorageShouldBeCorrect() {
			Git::Repo otherRepo(repo->workingDir());

			Git::Id      id("c56dada", *storage);
			Git::Id otherId("c56dada", *otherRepo.storages()[0]);

			QVERIFY( (id == otherId));
			QVERIFY(!(id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void comparisonWithLargerIdShouldBeCorrect() {
			Git::Id      id("86e041d", *storage);
			Git::Id otherId("c56dada", *storage);

			QVERIFY(!(id == otherId));
			QVERIFY( (id != otherId));
			QVERIFY( (id <  otherId));
		}

		void comparisonWithSmallerIdShouldBeCorrect() {
			Git::Id      id("86e041d", *storage);
			Git::Id otherId("5b36b1f", *storage);

			QVERIFY(!(id == otherId));
			QVERIFY( (id != otherId));
			QVERIFY(!(id <  otherId));
		}

		void shouldProduceCorrectHash() {
			Git::Id id("c56dada", *storage);

			QCOMPARE(qHash(id), (unsigned)0xc56dada2);
		}

		void invalidatedShouldBeInvalid() {
			Git::Id id("c56dada", *storage);
			id.invalidate();

			QVERIFY(!id.isValid());
		}

		void invalidatedShouldHaveId() {
			Git::Id id("c56dada", *storage);
			id.invalidate();

			QVERIFY(!id.d->sha1.isEmpty());
		}

		void invalidatedShouldNotHaveStorage() {
			Git::Id id("c56dada", *storage);
			id.invalidate();

			QVERIFY(!id.d->storage);
		}
};

QTEST_KDEMAIN_CORE(IdTest)

#include "IdTest.moc"
