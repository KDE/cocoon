#ifndef REPO_H
#define REPO_H

#include <QObject>



namespace Git {



enum RepoOptions {
	Bare
};
Q_ENUMS(RepoOptions)



class Repo : public QObject
{
	Q_OBJECT

	public:
		explicit Repo(QObject *parent = 0);

	// static
		static bool containsRepository(const QString &path);

	private:
		QString workingDir;
};

}

#endif // REPO_H
