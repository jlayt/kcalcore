/*
    This file is part of Akonadi Contact.

    Copyright (c) 2009 Tobias Koenig <tokoe@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "contactviewer.h"

#include <akonadi/item.h>
#include <akonadi/itemfetchscope.h>
#include <kabc/addressee.h>
#include <kcolorscheme.h>
#include <kglobal.h>
#include <kicon.h>
#include <klocale.h>
#include <kstringhandler.h>
#include <ktextbrowser.h>

#include <QtGui/QVBoxLayout>

using namespace Akonadi;

static QString contactAsHtml( const KABC::Addressee &contact );

class ContactViewer::Private
{
  public:
    Private( ContactViewer *parent )
      : mParent( parent )
    {
    }

    void slotMailClicked( const QString&, const QString &email )
    {
      QString name, address;

      // remove the 'mailto:' and split into name and email address
      KABC::Addressee::parseEmailAddress( email.mid( 7 ), name, address );

      emit mParent->emailClicked( name, address );
    }

    void slotUrlClicked( const QString &urlString )
    {
      QUrl url( urlString );

      if ( url.scheme() == QLatin1String( "http" ) ||
           url.scheme() == QLatin1String( "https" ) ) {
        emit mParent->urlClicked( urlString );
      } else if ( url.scheme() == QLatin1String( "phone" ) ) {
        const int pos = url.queryItemValue( QLatin1String( "index" ) ).toInt();

        const KABC::PhoneNumber::List numbers = mCurrentContact.phoneNumbers();
        if ( pos < numbers.count() ) {
          emit mParent->phoneNumberClicked( numbers.at( pos ) );
        }
      } else if ( url.scheme() == QLatin1String( "address" ) ) {
        const int pos = url.queryItemValue( QLatin1String( "index" ) ).toInt();

        const KABC::Address::List addresses = mCurrentContact.addresses();
        if ( pos < addresses.count() ) {
          emit mParent->addressClicked( addresses.at( pos ) );
        }
      }
    }

    ContactViewer *mParent;
    KTextBrowser *mBrowser;
    KABC::Addressee mCurrentContact;
};

ContactViewer::ContactViewer( QWidget *parent )
  : QWidget( parent ), d( new Private( this ) )
{
  QVBoxLayout *layout = new QVBoxLayout( this );

  d->mBrowser = new KTextBrowser;
  d->mBrowser->setNotifyClick( true );

  connect( d->mBrowser, SIGNAL( mailClick( const QString&, const QString& ) ),
           this, SLOT( slotMailClicked( const QString&, const QString& ) ) );
  connect( d->mBrowser, SIGNAL( urlClick( const QString& ) ),
           this, SLOT( slotUrlClicked( const QString& ) ) );

  layout->addWidget( d->mBrowser );

  // always fetch full payload for contacts
  fetchScope().fetchFullPayload();
}

ContactViewer::~ContactViewer()
{
  delete d;
}

Akonadi::Item ContactViewer::contact() const
{
  return ItemMonitor::item();
}

void ContactViewer::setContact( const Akonadi::Item &contact )
{
  ItemMonitor::setItem( contact );
}

void ContactViewer::itemChanged( const Item &contactItem )
{
  if ( !contactItem.hasPayload<KABC::Addressee>() )
    return;

  static QPixmap defaultPixmap = KIcon( QLatin1String( "x-office-contact" ) ).pixmap( QSize( 100, 140 ) );

  d->mCurrentContact = contactItem.payload<KABC::Addressee>();

  setWindowTitle( i18n( "Contact %1", d->mCurrentContact.assembledName() ) );

  if ( d->mCurrentContact.photo().isIntern() ) {
    d->mBrowser->document()->addResource( QTextDocument::ImageResource,
                                          QUrl( QLatin1String( "contact_photo" ) ),
                                          d->mCurrentContact.photo().data() );
  } else {
    d->mBrowser->document()->addResource( QTextDocument::ImageResource,
                                          QUrl( QLatin1String( "contact_photo" ) ),
                                          defaultPixmap );
  }

  d->mBrowser->setHtml( contactAsHtml( d->mCurrentContact ) );
}

void ContactViewer::itemRemoved()
{
  d->mBrowser->clear();
}

static QString contactAsHtml( const KABC::Addressee &contact )
{
  // We'll be building a table to display the vCard in.
  // Each row of the table will be built using this string for its HTML.

  QString rowFmtStr = QString::fromLatin1(
        "<tr>"
        "<td align=\"right\" valign=\"top\" width=\"30%\"><b><font size=\"-1\" color=\"grey\">%1</font></b></td>\n"
        "<td align=\"left\" valign=\"top\" width=\"70%\"><font size=\"-1\">%2</font></td>\n"
        "</tr>\n"
        );

  // Build the table's rows here
  QString dynamicPart;

  // Birthday
  const QDate date = contact.birthday().date();

  if ( date.isValid() )
    dynamicPart += rowFmtStr
      .arg( KABC::Addressee::birthdayLabel() )
      .arg( KGlobal::locale()->formatDate( date, KLocale::ShortDate ) );

  // Phone Numbers
  int counter = 0;
  foreach ( const KABC::PhoneNumber &number, contact.phoneNumbers() ) {
      const QString url = QString::fromLatin1( "<a href=\"phone:?index=%1\">%2</a>" ).arg( counter ).arg( number.number() );
      counter++;

      dynamicPart += rowFmtStr
        .arg( KABC::PhoneNumber::typeLabel( number.type() ).replace( QLatin1String( " " ), QLatin1String( "&nbsp;" ) ) )
        .arg( url );
  }

  // EMails
  foreach ( const QString &email, contact.emails() ) {
    QString type = i18nc( "a contact's email address", "Email" );

    const QString fullEmail = QString::fromLatin1( KUrl::toPercentEncoding( contact.fullEmail( email ) ) );

    dynamicPart += rowFmtStr.arg( type )
      .arg( QString::fromLatin1( "<a href=\"mailto:%1\">%2</a>" )
      .arg( fullEmail, email ) );
  }

  // Homepage
  if ( contact.url().isValid() ) {
    QString url = contact.url().url();
    if ( !url.startsWith( QLatin1String( "http://" ) ) && !url.startsWith( QLatin1String( "https://" ) ) )
      url = QLatin1String( "http://" ) + url;

    url = KStringHandler::tagUrls( url );
    dynamicPart += rowFmtStr.arg( i18n( "Homepage" ) ).arg( url );
  }

  // Blog Feed
  const QString blog = contact.custom( QLatin1String( "KADDRESSBOOK" ), QLatin1String( "BlogFeed" ) );
  if ( !blog.isEmpty() )
    dynamicPart += rowFmtStr.arg( i18n( "Blog Feed" ) ).arg( KStringHandler::tagUrls( blog ) );

  // Addresses
  counter = 0;
  foreach ( const KABC::Address &address, contact.addresses() ) {
    QString formattedAddress;

    if ( address.label().isEmpty() ) {
      formattedAddress = address.formattedAddress().trimmed();
    } else {
      formattedAddress = address.label();
    }

    formattedAddress = formattedAddress.replace( QLatin1Char( '\n' ), QLatin1String( "<br>" ) );

    const QString url = QString::fromLatin1( "<a href=\"address:?index=%1\">%2</a>" ).arg( counter).arg( formattedAddress );
    counter++;

    dynamicPart += rowFmtStr
      .arg( KABC::Address::typeLabel( address.type() ) )
      .arg( url );
  }

  // Note
  QString notes;
  if ( !contact.note().isEmpty() )
    notes = rowFmtStr.arg( i18n( "Notes" ) ).arg( contact.note().replace( QLatin1Char( '\n' ), QLatin1String( "<br>" ) ) ) ;

  // Custom Data
  QString customData;
  static QMap<QString, QString> titleMap;
  if ( titleMap.isEmpty() ) {
    titleMap.insert( QLatin1String( "Department" ), i18n( "Department" ) );
    titleMap.insert( QLatin1String( "Profession" ), i18n( "Profession" ) );
    titleMap.insert( QLatin1String( "AssistantsName" ), i18n( "Assistant's Name" ) );
    titleMap.insert( QLatin1String( "ManagersName" ), i18n( "Manager's Name" ) );
    titleMap.insert( QLatin1String( "SpousesName" ), i18nc( "Wife/Husband/...", "Partner's Name" ) );
    titleMap.insert( QLatin1String( "Office" ), i18n( "Office" ) );
    titleMap.insert( QLatin1String( "IMAddress" ), i18n( "IM Address" ) );
    titleMap.insert( QLatin1String( "Anniversary" ), i18n( "Anniversary" ) );
  }

  static QSet<QString> blacklistedKeys;
  if ( blacklistedKeys.isEmpty() ) {
    blacklistedKeys.insert( QLatin1String( "CRYPTOPROTOPREF" ) );
    blacklistedKeys.insert( QLatin1String( "OPENPGPFP" ) );
    blacklistedKeys.insert( QLatin1String( "SMIMEFP" ) );
    blacklistedKeys.insert( QLatin1String( "CRYPTOSIGNPREF" ) );
    blacklistedKeys.insert( QLatin1String( "CRYPTOENCRYPTPREF" ) );
  }

  if ( !contact.customs().empty() ) {
    const QStringList customs = contact.customs();
    foreach ( QString custom, customs ) { //krazy:exclude=foreach
      if ( custom.startsWith( QLatin1String( "KADDRESSBOOK-" ) ) ) {
        custom.remove( QLatin1String( "KADDRESSBOOK-X-" ) );
        custom.remove( QLatin1String( "KADDRESSBOOK-" ) );

        int pos = custom.indexOf( QLatin1Char( ':' ) );
        QString key = custom.left( pos );
        QString value = custom.mid( pos + 1 );

        // convert anniversary correctly
        if ( key == QLatin1String( "Anniversary" ) ) {
          const QDateTime dateTime = QDateTime::fromString( value, Qt::ISODate );
          value = KGlobal::locale()->formatDate( dateTime.date(), KLocale::ShortDate );
        }

        // blog is handled separated
        if ( key == QLatin1String( "BlogFeed" ) )
          continue;

        if ( blacklistedKeys.contains( key ) )
          continue;

        const QMap<QString, QString>::ConstIterator keyIt = titleMap.constFind( key );
        if ( keyIt != titleMap.constEnd() )
          key = keyIt.value();

        customData += rowFmtStr.arg( key ).arg( value ) ;
      }
    }
  }

  // Assemble all parts
  QString role = contact.title();
  if ( role.isEmpty() )
    role = contact.role();
  if ( role.isEmpty() )
    role = contact.custom( QLatin1String( "KADDRESSBOOK" ), QLatin1String( "X-Profession" ) );

  QString strAddr = QString::fromLatin1(
    "<div align=\"center\">"
    "<table cellpadding=\"3\" cellspacing=\"0\">"
    "<tr>"
    "<td align=\"right\" valign=\"top\" width=\"30%\" rowspan=\"3\">"
    "<img src=\"%1\" width=\"75\" height=\"105\" vspace=\"1\">" // image
    "</td>"
    "<td align=\"left\" width=\"70%\"><font size=\"+2\"><b>%2</b></font></td>" // name
    "</tr>"
    "<tr>"
    "<td align=\"left\" width=\"70%\">%3</td>"  // role
    "</tr>"
    "<tr>"
    "<td align=\"left\" width=\"70%\">%4</td>"  // organization
    "</tr>")
      .arg( QLatin1String( "contact_photo" ) )
      .arg( contact.realName() )
      .arg( role )
      .arg( contact.organization() );

  strAddr.append( dynamicPart );
  strAddr.append( notes );
  strAddr.append( customData );
  strAddr.append( QString::fromLatin1( "</table></div>\n" ) );

  const QString document = QString::fromLatin1(
    "<html>"
    "<head>"
    " <style type=\"text/css\">"
    "  a {text-decoration:none; color:%1}"
    " </style>"
    "</head>"
    "<body text=\"%1\" bgcolor=\"%2\">" // text and background color
    "%3" // contact part
    "</body>"
    "</html>" )
     .arg( KColorScheme( QPalette::Active, KColorScheme::View ).foreground().color().name() )
     .arg( KColorScheme( QPalette::Active, KColorScheme::View ).background().color().name() )
     .arg( strAddr );

  return document;
}

#include "contactviewer.moc"