#include "Repo.h"



using namespace Git;



Repo::Repo(QObject *parent)
	: QObject(parent)
{
}

bool Repo::containsRepository(const QString &path)
{
	return false;
}

#include "Repo.moc"
