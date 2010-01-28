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
#include "Git/RawObject.h"



class RawObjectTest : public GitTestBase
{
	Q_OBJECT

	Git::RawObject *object;
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

			QString id = repo->commits()[0]->id();
			object = new Git::RawObject(id, storage);
		}

		void shouldDetermineHeaderValiditiy_data() {
			QTest::addColumn<QString>("possibleHeader");
			QTest::addColumn<bool>("headerValid");

			QTest::newRow("no data")                   << QString()     << false;
			QTest::newRow("only header")               << "blob 0"      << true;
			QTest::newRow("standard commit header")    << "commit 123"  << true;
			QTest::newRow("standard tag header")       << "tag 123"     << true;
			QTest::newRow("standard tree header")      << "tree 123"    << true;
			QTest::newRow("with two null bytes")       << "blob 123"    << true;
			QTest::newRow("bogus data")                << QString()     << false;
			QTest::newRow("bogus header")              << QString()     << false;
			QTest::newRow("bogus type with size")      << QString()     << false;
		}

		void shouldDetermineHeaderValiditiy() {
			QFETCH(QString, possibleHeader);
			QFETCH(bool, headerValid);

			QCOMPARE(Git::RawObject::isValidHeader(possibleHeader), headerValid);
		}

		void shouldExtractHeaderCorrectly_data() {
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QString>("extractedHeader");

			QTest::newRow("no data")                   << QByteArray()                           << QString();
			QTest::newRow("only header")               << QByteArray("blob 0", 6)                << "blob 0";
			QTest::newRow("standard commit header")    << QByteArray("commit 123\0tree ", 16)    << "commit 123";
			QTest::newRow("standard tag header")       << QByteArray("tag 123\0foo", 11)         << "tag 123";
			QTest::newRow("standard tree header")      << QByteArray("tree 123\0foo", 12)        << "tree 123";
			QTest::newRow("with two null bytes")       << QByteArray("blob 123\0foo\0bar", 16)   << "blob 123";
			QTest::newRow("bogus data")                << QByteArray("foo\nbar\nbaz", 11)        << QString();
			QTest::newRow("bogus header")              << QByteArray("foo\nbar\0baz", 11)        << QString();
			QTest::newRow("bogus type with size")      << QByteArray("foo 123\0baz", 11)         << QString();
		}

		void shouldExtractHeaderCorrectly() {
			QFETCH(QByteArray, data);
			QFETCH(QString, extractedHeader);

			QCOMPARE(Git::RawObject::extractHeaderForm(data), extractedHeader);
		}

		void shouldExtractObjectTypeCorrectly_data() {
			QTest::addColumn<QString>("header");
			QTest::addColumn<QString>("objectType");

			QTest::newRow("invalid header")    << QString()    << QString();
			QTest::newRow("recognizes blob")   << "blob 123"   << "blob";
			QTest::newRow("recognizes commit") << "commit 123" << "commit";
			QTest::newRow("recognizes tag")    << "tag 123"    << "tag";
			QTest::newRow("recognizes tree")   << "tree 123"   << "tree";
		}

		void shouldExtractObjectTypeCorrectly() {
			QFETCH(QString, header);
			QFETCH(QString, objectType);

			QCOMPARE(Git::RawObject::extractObjectTypeFrom(header), objectType);
		}

		void shouldExtractObjectSizeCorrectly_data() {
			QTest::addColumn<QString>("header");
			QTest::addColumn<int>("objectSize");

			QTest::newRow("invalid header") << QString()       << -1;
			QTest::newRow("zero size")      << "blob 0"        << 0;
			QTest::newRow("small size")     << "blob 123"      << 123;
			QTest::newRow("medium size")    << "blob 123456"   << 123456;
			QTest::newRow("large size")     << "blob 12345678" << 12345678;
		}

		void shouldExtractObjectSizeCorrectly() {
			QFETCH(QString, header);
			QFETCH(int, objectSize);

			QCOMPARE(Git::RawObject::extractObjectSizeFrom(header), objectSize);
		}

		void shouldDetermineWhetherGivenDataIsOnlyHeaderOrMore_data() {
			QTest::addColumn<QByteArray>("rawData");
			QTest::addColumn<bool>("isOnlyHeader");

			QTest::newRow("empty data")       << QByteArray()                        << false;
			QTest::newRow("only header")      << QByteArray("commit 123")            << true;
			QTest::newRow("more than header") << QByteArray("commit 123\0tree ", 16) << false;
		}

		void shouldDetermineWhetherGivenDataIsOnlyHeaderOrMore() {
			QFETCH(QByteArray, rawData);
			QFETCH(bool, isOnlyHeader);

			QCOMPARE(Git::RawObject::isOnlyHeader(rawData), isOnlyHeader);
		}

		void testIsBlob() {
			object->m_type = "blob";

			QVERIFY( object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsCommit() {
			object->m_type = "commit";

			QVERIFY(!object->isBlob());
			QVERIFY( object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsTag() {
			object->m_type = "tag";

			QVERIFY(!object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY( object->isTag());
			QVERIFY(!object->isTree());
		}

		void testIsTree() {
			object->m_type = "tree";

			QVERIFY(!object->isBlob());
			QVERIFY(!object->isCommit());
			QVERIFY(!object->isTag());
			QVERIFY( object->isTree());
		}

		void shouldOnlyPopulateHeaderOnCreation() {
			QVERIFY( object->m_size != -1);
			QVERIFY(!object->m_type.isNull());
			QVERIFY( object->m_data.isNull());
		}

		void shouldPopulateFullyOnDataAccess() {
			QVERIFY(!object->data().isNull());
		}

		void dataShouldHaveTheSizeWrittenInTheHeader() {
			QCOMPARE(object->data().size(), object->m_size);
		}
};

QTEST_KDEMAIN_CORE(RawObjectTest);

#include "RawObjectTest.moc"
