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

#include "Tree.h"
#include "Tree_p.h"

#include "Blob.h"
#include "Repo.h"

#include <QDebug>
#include <QStringList>

using namespace Git;



Tree::Tree()
	: RawObject()
{
}

Tree::Tree(const Id& id, Repo &repo)
	: RawObject(id, repo)
	, d(new TreePrivate(*RawObject::d))
{
}

Tree::Tree(const Tree &other)
	: RawObject(other)
{
}

Tree::Tree(const RawObject &other)
	: RawObject(other)
{
	Q_ASSERT(other.isTree());
}

Tree::~Tree()
{
}



const QList<Blob> Tree::blobs()
{
	QList<Blob> result;

	foreach (const RawObject &entry, entries()) {
		if(entry.isBlob()) {
			result << entry;
		}
	}

	return result;
}

const QMap<QString, Blob> Tree::blobsByName()
{
	QMap<QString, Blob> result;

	foreach (const Blob &entry, blobs()) {
		result[nameFor(entry)] = entry;
	}

	return result;
}

const QList<RawObject> Tree::entries()
{
	fillFromString(this, data());
	return d->entries;
}

const QMap<QString, RawObject> Tree::entriesByName()
{
	QMap<QString, RawObject> result;

	foreach (const RawObject &entry, entries()) {
		result[nameFor(entry)] = entry;
	}


	return result;
}

void Tree::fillFromString(Tree *tree, const QByteArray &raw)
{
	// if commit has already been filled
	if (tree->d->entries.size() > 0) {
		return;
	}

	qDebug() << "fill tree" << tree->id().toString();

	int pos = 0;
	int modeLen = 0;
	int nameLen = 0;

	tree->d->entries.clear();
	tree->d->entryModes.clear();
	tree->d->entryNames.clear();

	while(pos < raw.size()) {
		modeLen = raw.indexOf(" ", pos)-pos;
		QString mode = raw.mid(pos, modeLen);
		pos += modeLen+1; // skip the mode and the " "
		nameLen = raw.indexOf('\0', pos)-pos;
		QString name = raw.mid(pos, nameLen);
		pos += nameLen+1; // skip the file name + "\0"
		QString id = raw.mid(pos, 20).toHex();
		pos += 20; // skip the id

		RawObject* entry = tree->repo().object(tree->repo().idFor(id));
		tree->d->entries << *entry;
		tree->d->entryModes[id] = mode;
		tree->d->entryNames[id] = name;
	}

}

const QString Tree::nameFor(const QString &id)
{
	fillFromString(this, data());
	return d->entryNames[id];
}

const QString Tree::nameFor(const RawObject &object)
{
	return nameFor(object.id().toSha1String());
}

const QList<Tree> Tree::trees()
{
	QList<Tree> result;

	foreach (const RawObject &entry, entries()) {
		if (entry.isTree()) {
			result << entry;
		}
	}

	return result;
}

Tree& Tree::operator=(const Tree &other)
{
	RawObject::operator=(other);

	return *this;
}

const QMap<QString, Tree> Tree::treesByName()
{
	QMap<QString, Tree> result;

	foreach(const Tree entry, trees()) {
		result[nameFor(entry)] = entry;
	}

	return result;
}

#include "Tree.moc"
