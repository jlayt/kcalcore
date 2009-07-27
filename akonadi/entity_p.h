/*
    Copyright (c) 2008 Tobias Koenig <tokoe@kde.org>

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

#ifndef ENTITY_P_H
#define ENTITY_P_H

#include "entity.h"

#include <QtCore/QSet>
#include <QtCore/QSharedData>
#include <QtCore/QString>

#define AKONADI_DEFINE_PRIVATE( Class ) \
Class##Private* Class ::d_func() { return reinterpret_cast<Class##Private *>( d_ptr.data() ); } \
const Class##Private* Class ::d_func() const { return reinterpret_cast<const Class##Private *>( d_ptr.data() ); }

namespace Akonadi {

/**
 * @internal
 */
class EntityPrivate : public QSharedData
{
  public:
    EntityPrivate( Entity::Id id = -1 )
      : mId( id )
    {
    }

    virtual ~EntityPrivate()
    {
      qDeleteAll( mAttributes );
    }

    EntityPrivate( const EntityPrivate &other )
      : QSharedData( other )
    {
      mId = other.mId;
      mRemoteId = other.mRemoteId;
      foreach ( Attribute* attr, other.mAttributes )
        mAttributes.insert( attr->type(), attr->clone() );
      mDeletedAttributes = other.mDeletedAttributes;
    }

    virtual void resetChangeLog()
    {
      mDeletedAttributes.clear();
    }

    virtual EntityPrivate *clone() const = 0;

    Entity::Id mId;
    QString mRemoteId;
    QHash<QByteArray, Attribute*> mAttributes;
    QSet<QByteArray> mDeletedAttributes;
};

}

/**
 * @internal
 *
 * This template specialization is used to change the detach
 * behaviour of QSharedDataPointer to match our needs.
 */
template <>
Q_INLINE_TEMPLATE void QSharedDataPointer<Akonadi::EntityPrivate>::detach()
{
    if (d && d->ref != 1)
    {
        Akonadi::EntityPrivate *x = d->clone();
        x->ref.ref();
        if (!d->ref.deref())
            delete d;
        d = x;
    }
}

#endif