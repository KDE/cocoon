#include "cocoon.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = "%{VERSION}";

int main(int argc, char **argv)
{
    KAboutData about("cocoon", 0, ki18n("Cocoon"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 %{AUTHOR}"), KLocalizedString(), 0, "%{EMAIL}");
    about.addAuthor( ki18n("%{AUTHOR}"), KLocalizedString(), "%{EMAIL}" );
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    Cocoon *widget = new Cocoon;

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(Cocoon);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            //cocoon *widget = new cocoon;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++)
            {
                //cocoon *widget = new cocoon;
                widget->show();
            }
        }
        args->clear();
    }

    return app.exec();
}
