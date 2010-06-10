
#include "MainWindow.h"
#include "OpenRepositoryDialog.h"

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

int main(int argc, char **argv)
{
	KAboutData about("cocoon", 0, ki18n("Cocoon"), "0.1", ki18n("A Git GUI for KDE4"),
		KAboutData::License_GPL_V3, ki18n("(C) 2009-2010 Riyad Preukschas"), KLocalizedString(), "riyad@informatik.uni-bremen.de");
	about.addAuthor( ki18n("Riyad Preukschas"), KLocalizedString(), "riyad@informatik.uni-bremen.de" );
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
				mainWindow->setRepository(repoDialog->selectedRepositoryPath());
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
