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

#include "Git/LooseStorage.h"



class LooseStorageTest : public GitTestBase
{
	Q_OBJECT

	Git::LooseStorage *storage;

	private slots:
		void initTestCase() {
			GitTestBase::initTestCase();

			storage = 0;

			writeToFile("some_file.txt", "foo\nbar\baz");

			QProcess::execute("git", gitBasicOpts() << "add" << "some_file.txt");
			QProcess::execute("git", gitBasicOpts() << "commit" << "-m" << "Added a file.");
		}

		void init() {
			GitTestBase::init();
			storage = new Git::LooseStorage(repo);
		}

		void cleanup() {
			delete storage;
			GitTestBase::cleanup();
		}

		void shouldHaveObjectsDir() {
			QVERIFY(!storage->m_objectsDir.path().isEmpty());
		}

		void shouldHaveCorrectObjectsDir() {
			QCOMPARE(storage->m_objectsDir.path(), QString("%1/.git/objects").arg(repo->workingDir()));
		}

		void testSourceIsCorrect() {
			QString sourcePath = storage->sourceFor("1234567890123456789012345678901234567890");

			QCOMPARE(sourcePath, QString("%1/.git/objects/12/34567890123456789012345678901234567890").arg(repo->workingDir()));
		}

		void testInflationIsWorking() {
			QString id = repo->commits()[0]->id();
			QByteArray rawData = storage->rawDataFor(id);

			QCOMPARE(QTest::toHexRepresentation(rawData, 16), QTest::toHexRepresentation("commit 212\0tree ", 16));
			QCOMPARE(rawData.size(), QString("commit 212").length() + 1 + 212);
		}

		void shouldOnlyExtractHeader() {
			QString id = repo->commits()[0]->id();
			QString rawHeader = storage->rawHeaderFor(id);

			QCOMPARE(rawHeader, QString("commit 212"));
		}

		void shouldCacheRawDataBetweenQueries() {
			QString id = repo->commits()[0]->id();
			const char* pData1 = storage->rawDataFor(id).data();
			const char* pData2 = storage->rawDataFor(id).data();

			QVERIFY(pData1 == pData2);
		}

		void shouldCacheRawObjectsBetweenQueries() {
			QString id = repo->commits()[0]->id();
			Git::RawObject* pObject1 = storage->rawObjectFor(id);
			Git::RawObject* pObject2 = storage->rawObjectFor(id);

			QVERIFY(pObject1 == pObject2);
		}

		void shouldCacheRawHeadersBetweenQueries() {
			QString id = repo->commits()[0]->id();
			const char* pData1 = storage->rawHeaderFor(id).data();
			const char* pData2 = storage->rawHeaderFor(id).data();

			QVERIFY(pData1 == pData2);

			const QByteArray header1 = storage->rawHeaderFor(id);
			const QByteArray header2 = storage->rawHeaderFor(id);

			QVERIFY(header1.data() == header2.data());
		}

		void shouldReplaceRawHeadersWithRawData() {
			QString id = repo->commits()[0]->id();
			const QByteArray header = storage->rawHeaderFor(id);
			const QByteArray data = storage->rawDataFor(id);

			QVERIFY(header.data() != data.data());
		}
};

QTEST_KDEMAIN_CORE(LooseStorageTest);



#include "LooseStorageTest.moc"
