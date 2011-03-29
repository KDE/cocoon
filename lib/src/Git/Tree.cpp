/*
	Cocoon - A GUI for Git.
	Copyright (C) 2009-2011  Riyad Preukschas <riyad@informatik.uni-bremen.de>

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

#include "Tree.h"
#include "Tree_p.h"

#include "Blob.h"
#include "Repo.h"

#include <QDebug>
#include <QStringList>

using namespace Git;



Tree::Tree()
	: RawObject()
	, d(new TreePrivate(*RawObject::d))
{
}

Tree::Tree(const Id& id, Repo &repo)
	: RawObject(id, repo)
	, d(new TreePrivate(*RawObject::d))
{
}

Tree::Tree(const Tree &other)
	: RawObject(other)
	, d(other.d)
{
}

Tree::Tree(const RawObject &other)
	: RawObject(other)
	, d(new TreePrivate(*other.d))
{
	Q_ASSERT(other.isTree());
}

Tree::~Tree()
{
}



const QList<Blob> Tree::blobs() const
{
	QList<Blob> result;

	foreach (const RawObject &entry, entries()) {
		if(entry.isBlob()) {
			result << entry.toBlob();
		}
	}

	return result;
}

const QMap<QString, Blob> Tree::blobsByName() const
{
	QMap<QString, Blob> result;

	foreach (const Blob &entry, blobs()) {
		result[nameFor(entry)] = entry;
	}

	return result;
}

const QList<RawObject> Tree::entries() const
{
	((Tree*)this)->lazyLoad();  // non-const access
	return d->entries;
}

const QMap<QString, RawObject> Tree::entriesByName() const
{
	QMap<QString, RawObject> result;

	foreach (const RawObject &entry, entries()) {
		result[nameFor(entry)] = entry;
	}


	return result;
}

void Tree::fillFromString(const QByteArray &raw)
{
	qDebug() << "fill tree" << id().toString();

	int pos = 0;
	int modeLen = 0;
	int nameLen = 0;

	d->entries.clear();
	d->entryModes.clear();
	d->entryNames.clear();

	while(pos < raw.size()) {
		modeLen = raw.indexOf(" ", pos)-pos;
		QString mode = raw.mid(pos, modeLen);
		pos += modeLen+1; // skip the mode and the " "
		nameLen = raw.indexOf('\0', pos)-pos;
		QString name = raw.mid(pos, nameLen);
		pos += nameLen+1; // skip the file name + "\0"
		QString idString = raw.mid(pos, 20).toHex();
		pos += 20; // skip the id

		RawObject &entry = repo().idFor(idString).object();
		d->entries << entry;
		d->entryModes[entry.id()] = mode;
		d->entryNames[entry.id()] = name;
	}
}

Tree& Tree::invalid()
{
	static Tree invalid;
	return invalid;
}

void Tree::lazyLoad()
{
	// if tree has already been filled
	if (d->entries.size() > 0) {
		return;
	}

	fillFromString(data());
}

const QString& Tree::nameFor(const Id &id) const
{
	((Tree*)this)->lazyLoad(); // non-const access
	return d->entryNames[id];
}

const QString& Tree::nameFor(const RawObject &object) const
{
	return nameFor(object.id());
}

Tree& Tree::operator=(const Tree &other)
{
	RawObject::operator=(other);

	d = other.d;

	return *this;
}

const QList<Tree> Tree::trees() const
{
	QList<Tree> result;

	foreach (const RawObject &entry, entries()) {
		if (entry.isTree()) {
			result << entry.toTree();
		}
	}

	return result;
}

const QMap<QString, Tree> Tree::treesByName() const
{
	QMap<QString, Tree> result;

	foreach(const Tree& entry, trees()) {
		result[nameFor(entry)] = entry;
	}

	return result;
}

#include "Tree.moc"
