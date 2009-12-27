#include "cocoon.h"

#include "MainWindow.h"
#include "OpenRepositoryDialog.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] = I18N_NOOP("A GUI for Git.");

static const char version[] = "%{VERSION}";

int main(int argc, char **argv)
{
	KAboutData about("cocoon", 0, ki18n("Cocoon"), version, ki18n(description),
		KAboutData::License_GPL_V3, ki18n("(C) 2009 %{AUTHOR}"), KLocalizedString(), 0, "%{EMAIL}");
	about.addAuthor( ki18n("%{AUTHOR}"), KLocalizedString(), "%{EMAIL}" );
	KCmdLineArgs::init(argc, argv, &about);

	KCmdLineOptions options;
	options.add("+[URL]", ki18n( "Document to open" ));
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication app;

	MainWindow *mainWindow = new MainWindow();

	// see if we are starting with session management
	if (app.isSessionRestored()) {
		//! \todo RESTORE(MainWindow);
	} else {
		// no session.. just start up normally
		KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		if (args->count() == 0) {
			OpenRepositoryDialog *repoDialog = new OpenRepositoryDialog();
			repoDialog->exec();

			if(repoDialog->result() == QDialog::Accepted) {
				//mainWindow->setRepository(Git::Repository(repoDialog->selectedRepositoryPath()));
				mainWindow->show();
				delete repoDialog;
			} else {
				return 0;
			}
		} else {
			//! \todo process command line arguments
		}
		args->clear();
	}

	return app.exec();
}
