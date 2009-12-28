#ifndef REPO_H
#define REPO_H

#include <QObject>



namespace Git {



enum RepoOption {
	None = 0,
	Bare = 1<<1
};
Q_DECLARE_FLAGS(RepoOptions, RepoOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(RepoOptions)



class Repo : public QObject
{
	Q_OBJECT

	public:
		explicit Repo(QObject *parent = 0);

	// static
		static bool containsRepository(const QString &path, const RepoOptions options = None);

	private:
		QString workingDir;
};

}

#endif // REPO_H
