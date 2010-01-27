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
			QString id = repo->commits()[0]->id();
			QString sourcePath = storage->sourceFor(id);

			QCOMPARE(sourcePath, QString("%1/.git/objects/%2/%3").arg(repo->workingDir()).arg(id.left(2)).arg(id.mid(2)));
		}

		void testInflationIsWorking() {
			QString id = repo->commits()[0]->id();
			QByteArray data = storage->rawDataFor(id);

			QCOMPARE(QString::fromLatin1(data.data(), 11), QString::fromLatin1("commit 212\0", 11));
			QCOMPARE(data.size(), QString("commit 212").length() + 1 + 212);
		}

		void shouldDetermineHeaderValiditiy_data() {
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<bool>("headerValid");

			QTest::newRow("no data")                   << QByteArray()                           << false;
			QTest::newRow("only header")               << QByteArray("blob 0\0", 7)              << true;
			QTest::newRow("standard commit header")    << QByteArray("commit 123\0tree ", 16)    << true;
			QTest::newRow("standard tag header")       << QByteArray("tag 123\0foo", 11)         << true;
			QTest::newRow("standard tree header")      << QByteArray("tree 123\0foo", 12)        << true;
			QTest::newRow("with two null bytes")       << QByteArray("blob 123\0foo\0bar", 16)   << true;
			QTest::newRow("bogus data")                << QByteArray("foo\nbar\nbaz", 11)        << false;
			QTest::newRow("bogus header")              << QByteArray("foo\nbar\0baz", 11)        << false;
			QTest::newRow("bogus type with size")      << QByteArray("foo 123\0baz", 11)         << false;
		}

		void shouldDetermineHeaderValiditiy() {
			QFETCH(QByteArray, data);
			QFETCH(bool, headerValid);

			QCOMPARE(storage->hasValidHeader(data), headerValid);
		}

		void shouldExtractHeaderCorrectly_data() {
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QString>("extractedHeader");

			QTest::newRow("no data")                   << QByteArray()                           << QString();
			QTest::newRow("only header")               << QByteArray("blob 0\0", 7)              << "blob 0";
			QTest::newRow("standard header")           << QByteArray("commit 123\0tree ", 15)    << "commit 123";
			QTest::newRow("with two null bytes")       << QByteArray("commit 123\0foo\0bar", 17) << "commit 123";
			QTest::newRow("bogus data")                << QByteArray("foo\nbar\nbaz", 11)        << QString();
			QTest::newRow("bogus data with null byte") << QByteArray("foo\nbar\0baz", 11)        << QString();
		}

		void shouldExtractHeaderCorrectly() {
			QFETCH(QByteArray, data);
			QFETCH(QString, extractedHeader);

			QCOMPARE(storage->extractHeaderForm(data), extractedHeader);
		}

		void shouldExtractObjectTypeCorrectly_data() {
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QString>("objectType");

			QTest::newRow("no data")                   << QByteArray()                           << QString();
			QTest::newRow("only header")               << QByteArray("blob 0\0", 7)              << "blob";
			QTest::newRow("standard commit header")    << QByteArray("commit 123\0tree ", 16)    << "commit";
			QTest::newRow("standard tag header")       << QByteArray("tag 123\0foo", 11)         << "tag";
			QTest::newRow("standard tree header")      << QByteArray("tree 123\0foo", 12)        << "tree";
			QTest::newRow("with two null bytes")       << QByteArray("blob 123\0foo\0bar", 16)   << "blob";
			QTest::newRow("bogus data")                << QByteArray("foo\nbar\nbaz", 11)        << QString();
			QTest::newRow("bogus header")              << QByteArray("foo\nbar\0baz", 11)        << QString();
			QTest::newRow("bogus type with size")      << QByteArray("foo 123\0baz", 11)         << QString();
		}

		void shouldExtractObjectTypeCorrectly() {
			QFETCH(QByteArray, data);
			QFETCH(QString, objectType);

			QCOMPARE(storage->extractObjectTypeFrom(data), objectType);
		}

		void shouldExtractObjectSizeCorrectly_data() {
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<int>("size");

			QTest::newRow("no data")                   << QByteArray()                           << -1;
			QTest::newRow("only header")               << QByteArray("blob 0\0", 7)              << 0;
			QTest::newRow("standard commit header")    << QByteArray("commit 123\0tree ", 16)    << 123;
			QTest::newRow("standard tag header")       << QByteArray("tag 123\0foo", 11)         << 123;
			QTest::newRow("standard tree header")      << QByteArray("tree 123\0foo", 12)        << 123;
			QTest::newRow("with two null bytes")       << QByteArray("blob 123\0foo\0bar", 16)   << 123;
			QTest::newRow("bogus data")                << QByteArray("foo\nbar\nbaz", 11)        << -1;
			QTest::newRow("bogus header")              << QByteArray("foo\nbar\0baz", 11)        << -1;
			QTest::newRow("bogus type with size")      << QByteArray("foo 123\0baz", 11)         << -1;
		}

		void shouldExtractObjectSizeCorrectly() {
			QFETCH(QByteArray, data);
			QFETCH(int, size);

			QCOMPARE(storage->extractObjectSizeFrom(data), size);
		}
};

QTEST_KDEMAIN_CORE(LooseStorageTest);

#include "LooseStorageTest.moc"
