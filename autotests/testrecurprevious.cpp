/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>
  Copyright (C) 2005 Reinhold Kainhofer <reinhold@kainhofe.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "filestorage.h"
#include "memorycalendar.h"

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <qdebug.h>
#include <ksystemtimezone.h>
#include <KLocalizedString>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

using namespace KCalCore;

static QString dumpTime(const QDateTime &dt, const QTimeZone &viewZone);

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "verbose" , i18n("Verbose output")));
    parser.addPositionalArgument("input", i18n("Name of input file"));
    parser.addPositionalArgument("output", i18n("optional name of output file for the recurrence dates"));

    KAboutData about(QStringLiteral("testrecurrencenew"),
                     i18n("Load recurrence rules with the new class and print out debug messages"),
                     QStringLiteral("0.1"));

    about.setupCommandLine(&parser);
    KAboutData::setApplicationData(about);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testrecurrencenew"));
    QCoreApplication::setApplicationVersion("0.1");
    parser.process(app);
    about.processCommandLine(&parser);

    QStringList parsedArgs = parser.positionalArguments();

    if (parsedArgs.isEmpty()) {
        parser.showHelp();
    }
    QString input = parsedArgs[0];
    qDebug() << "Input file:" << input;

    QTextStream *outstream;
    outstream = 0;
    QString fn("");
    if (parsedArgs.count() > 1) {
        fn = parsedArgs[1];
        qDebug() << "We have a file name given:" << fn;
    }
    QFile outfile(fn);
    if (!fn.isEmpty() && outfile.open(QIODevice::WriteOnly)) {
        qDebug() << "Opened output file!!!";
        outstream = new QTextStream(&outfile);
    }

    MemoryCalendar::Ptr cal(new MemoryCalendar(QTimeZone::utc()));

    QTimeZone viewZone;
    FileStorage store(cal, input);
    if (!store.load()) {
        return 1;
    }
    QString tz = cal->nonKDECustomProperty("X-LibKCal-Testsuite-OutTZ");
    if (!tz.isEmpty()) {
        viewZone = QTimeZone(tz.toLatin1()))
    }

    Incidence::List inc = cal->incidences();

    for (Incidence::List::Iterator it = inc.begin(); it != inc.end(); ++it) {
        Incidence::Ptr incidence = *it;
        qDebug() << "*+*+*+*+*+*+*+*+*+*";
        qDebug() << " ->" << incidence->summary() << "<-";

        incidence->recurrence()->dump();

        QDateTime dt(incidence->recurrence()->endDateTime());
        int i = 0;
        if (outstream) {
            if (!dt.isValid()) {
                if (viewZone.isValid()) {
                    dt = QDateTime(QDate(2011, 1, 1), QTime(0, 0, 1), viewZone);
                } else {
                    dt = QDateTime(QDate(2011, 1, 1), QTime(0, 0, 1));
                }
            } else {
                dt = dt.addYears(2);
            }
            qDebug() << "-------------------------------------------";
            qDebug() << " *~*~*~*~ Starting with date:" << dumpTime(dt, viewZone);
            // Output to file for testing purposes
            while (dt.isValid() && i < 500) {
                ++i;
                dt = incidence->recurrence()->getPreviousDateTime(dt);
                if (dt.isValid()) {
                    (*outstream) << dumpTime(dt, viewZone) << endl;
                }
            }
        } else {
            if (!dt.isValid()) {
                dt = QDateTime(QDate(2005, 7, 31), QTime(23, 59, 59), QTimeZone::utc());
            } else {
                dt = dt.addYears(2);
            }
            incidence->recurrence()->dump();
            qDebug() << "-------------------------------------------";
            qDebug() << " *~*~*~*~ Starting with date:" << dumpTime(dt, viewZone);
            // Output to konsole
            while (dt.isValid() && i < 50) {
                ++i;
                qDebug() << "-------------------------------------------";
                dt = incidence->recurrence()->getPreviousDateTime(dt);
                if (dt.isValid()) {
                    qDebug() << " *~*~*~*~ Previous date is:" << dumpTime(dt, viewZone);
                }
            }
        }
    }

    delete outstream;
    outfile.close();
    return 0;
}

QString dumpTime(const QDateTime &dt, const QTimeZone &viewZone)
{
    if (!dt.isValid()) {
        return QString();
    }
    QDateTime vdt = viewZone.isValid() ? dt.toTimeZone(viewZone) : dt;
    QString format;
    format = QStringLiteral("%Y-%m-%dT%H:%M:%S");
    // FIXME SecondOccurrance - Fix when supported in QDateTime
    //if (vdt.isSecondOccurrence()) {
    //    format += QStringLiteral(" %Z");
    //}
    if (vdt.timeSpec() != Qt::LocalTime) {
        format += QStringLiteral(" %:Z");
    }
    return vdt.toString(format);
}
