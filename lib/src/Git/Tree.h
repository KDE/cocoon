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

#ifndef TREE_H
#define TREE_H

#include "RawObject.h"
#include "Tree_p.h"

#include <QHash>
#include <QMap>



class TreeTest;

namespace Git {

class Blob;
class RawObject;



class KDE_EXPORT Tree : public RawObject
{
	Q_OBJECT

	public:
		explicit Tree(const Id& id, Repo &repo);

		const QList<Blob*> blobs();
		const QMap<QString, Blob*> blobsByName();
		const QList<RawObject*> entries();
		const QMap<QString, RawObject*> entriesByName();
		const QString nameFor(const RawObject &object);
		const QList<Tree*> trees();
		const QMap<QString, Tree*> treesByName();

	private:
		// static
		/**
		 * Populates the given tree with the data extracted from the raw data.
		 *
		 * @param tree The tree to be populated.
		 * @param raw The raw tree object data.
		 */
		static void fillFromString(Tree *tree, const QByteArray &raw);

	private:
		const QString nameFor(const QString &id);

	private:
		QSharedDataPointer<TreePrivate> d;

	friend class ::TreeTest;
};

}

#endif // TREE_H
