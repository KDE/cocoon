#include "Repo.h"



#include <QDir>



using namespace Git;



Repo::Repo(QObject *parent)
	: QObject(parent)
{
}

bool Repo::containsRepository(const QString &path, const RepoOptions options)
{
	QString repoPath = QDir(path).absolutePath();

	QString bareRepoPath;
	QString dotGitPath;

	if (!(options & Bare)) {
		dotGitPath = repoPath + "/.git";
		if (QDir(dotGitPath).exists()) {
			bareRepoPath = dotGitPath;
		}
	}

	if (bareRepoPath.isNull()) {
		bareRepoPath = repoPath;
	}

	QDir bareRepoDir = QDir(bareRepoPath);
	if(bareRepoDir.exists()) { // "*.git" directory exists
		QStringList dirs, files; // it should contain these dirs and files
		dirs << "branches" << "objects" << "refs";
		files << "config" << "description" << "index";

		QStringList entries = bareRepoDir.entryList();

		bool isRepo = true; // will become false if only one item is missing

		/** @todo actually distinguish between dirs and files */
		foreach(const QString &item, dirs + files) {
			isRepo = isRepo && entries.contains(item);
		}

		return isRepo;
	}

	return false;
}

#include "Repo.moc"
