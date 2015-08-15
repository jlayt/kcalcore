/*
  This file is part of the kcalcore library.

  Copyright (c) 1998 Preston Brown <pbrown@kde.org>
  Copyright (c) 2001,2003 Cornelius Schumacher <schumacher@kde.org>

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
/**
  @file
  This file is part of the API for handling calendar data and
  defines the MemoryCalendar class.

  Very simple implementation of a Calendar that is only in memory

  @author Preston Brown \<pbrown@kde.org\>
  @author Cornelius Schumacher \<schumacher@kde.org\>
 */
#ifndef KCALCORE_MEMORYCALENDAR_H
#define KCALCORE_MEMORYCALENDAR_H

#include "kcalcore_export.h"
#include "calendar.h"

#include <QTimeZone>

namespace KCalCore
{

class CalFormat;

/**
  @brief
  This class provides a calendar stored in memory.
*/
class KCALCORE_EXPORT MemoryCalendar : public Calendar
{
public:

    /**
      A shared pointer to a MemoryCalendar
    */
    typedef QSharedPointer<MemoryCalendar> Ptr;

    /**
      @copydoc Calendar::Calendar(const QTimeZone &)
    */
    explicit MemoryCalendar(const QTimeZone &timeZone);

    /**
      @copydoc Calendar::Calendar(const QByteArray &)
    */
    explicit MemoryCalendar(const QByteArray &timeZoneId);

    /**
      @copydoc Calendar::~Calendar()
    */
    ~MemoryCalendar();

    /**
      Clears out the current calendar, freeing all used memory etc. etc.
    */
    void close() Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteIncidence()
    */
    bool deleteIncidence(const Incidence::Ptr &incidence) Q_DECL_OVERRIDE;

    /**
       @copydoc Calendar::deleteIncidenceInstances
    */
    bool deleteIncidenceInstances(const Incidence::Ptr &incidence) Q_DECL_OVERRIDE;

    /**
       @copydoc Calendar::addIncidence()
    */
    bool addIncidence(const Incidence::Ptr &incidence) Q_DECL_OVERRIDE;

    // Event Specific Methods //

    /**
      @copydoc Calendar::addEvent()
    */
    bool addEvent(const Event::Ptr &event) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteEvent()
    */
    bool deleteEvent(const Event::Ptr &event) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteEventInstances()
    */
    bool deleteEventInstances(const Event::Ptr &event) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawEvents(EventSortField, SortDirection)const
    */
    Event::List rawEvents(
        EventSortField sortField = EventSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawEvents(const QDate &, const QDate &, const QTimeZone &, bool)const
    */
    Event::List rawEvents(const QDate &start, const QDate &end,
                          const QTimeZone &timeZone = QTimeZone(),
                          bool inclusive = false) const Q_DECL_OVERRIDE;

    /**
      Returns an unfiltered list of all Events which occur on the given date.

      @param date request unfiltered Event list for this QDate only.
      @param timeZone time zone to interpret @p date, or the calendar's
                      default time zone if none is specified
      @param sortField specifies the EventSortField.
      @param sortDirection specifies the SortDirection.

      @return the list of unfiltered Events occurring on the specified QDate.
    */
    Event::List rawEventsForDate(
        const QDate &date, const QTimeZone &timeZone = QTimeZone(),
        EventSortField sortField = EventSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawEventsForDate(const QDateTime &)const
    */
    Event::List rawEventsForDate(const QDateTime &dt) const Q_DECL_OVERRIDE;

    /**
     * Returns an incidence by identifier.
     * @see Incidence::instanceIdentifier()
     * @since 4.11
     */
    Incidence::Ptr instance(const QString &identifier) const;

    /**
      @copydoc Calendar::event()
    */
    Event::Ptr event(
        const QString &uid,
        const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedEvent()
    */
    Event::Ptr deletedEvent(
        const QString &uid, const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedEvents(EventSortField, SortDirection)const
    */
    Event::List deletedEvents(
        EventSortField sortField = EventSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::eventInstances(const Incidence::Ptr &, EventSortField, SortDirection)const
    */
    Event::List eventInstances(
        const Incidence::Ptr &event,
        EventSortField sortField = EventSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    // To-do Specific Methods //

    /**
      @copydoc Calendar::addTodo()
    */
    bool addTodo(const Todo::Ptr &todo) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteTodo()
    */
    bool deleteTodo(const Todo::Ptr &todo) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteTodoInstances()
    */
    bool deleteTodoInstances(const Todo::Ptr &todo) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawTodos(TodoSortField, SortDirection)const
    */
    Todo::List rawTodos(
        TodoSortField sortField = TodoSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
       @copydoc Calendar::rawTodos(const QDate &, const QDate &, const QTimeZone &, bool)const
    */
    Todo::List rawTodos(
        const QDate &start, const QDate &end,
        const QTimeZone &timeZone = QTimeZone(),
        bool inclusive = false) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawTodosForDate()
    */
    Todo::List rawTodosForDate(const QDate &date) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::todo()
    */
    Todo::Ptr todo(const QString &uid,
                   const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedTodo()
    */
    Todo::Ptr deletedTodo(const QString &uid, const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedTodos(TodoSortField, SortDirection)const
    */
    Todo::List deletedTodos(
        TodoSortField sortField = TodoSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::todoInstances(const Incidence::Ptr &, TodoSortField, SortDirection)const
    */
    Todo::List todoInstances(const Incidence::Ptr &todo,
                             TodoSortField sortField = TodoSortUnsorted,
                             SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    // Journal Specific Methods //

    /**
      @copydoc Calendar::addJournal()
    */
    bool addJournal(const Journal::Ptr &journal) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteJournal()
    */
    bool deleteJournal(const Journal::Ptr &journal) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deleteJournalInstances()
    */
    bool deleteJournalInstances(const Journal::Ptr &journal) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawJournals()
    */
    Journal::List rawJournals(
        JournalSortField sortField = JournalSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::rawJournalsForDate()
    */
    Journal::List rawJournalsForDate(const QDate &date) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::journal()
    */
    Journal::Ptr journal(const QString &uid,
                         const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedJournal()
    */
    Journal::Ptr deletedJournal(const QString &uid,
                                const QDateTime &recurrenceId = QDateTime()) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::deletedJournals(JournalSortField, SortDirection)const
    */
    Journal::List deletedJournals(
        JournalSortField sortField = JournalSortUnsorted,
        SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::journalInstances(const Incidence::Ptr &,
                                          JournalSortField, SortDirection)const
    */
    Journal::List journalInstances(const Incidence::Ptr &journal,
                                   JournalSortField sortField = JournalSortUnsorted,
                                   SortDirection sortDirection = SortDirectionAscending) const Q_DECL_OVERRIDE;

    // Alarm Specific Methods //

    /**
      @copydoc Calendar::alarms()
    */
    Alarm::List alarms(const QDateTime &from, const QDateTime &to, bool excludeBlockedAlarms = false) const Q_DECL_OVERRIDE;

    /**
      Return a list of Alarms that occur before the specified timestamp.

      @param to is the ending timestamp.
      @return the list of Alarms occurring before the specified QDateTime.
    */
    Alarm::List alarmsTo(const QDateTime &to) const;

    /**
      @copydoc Calendar::incidenceUpdate(const QString &,const QDateTime &)
    */
    void incidenceUpdate(const QString &uid, const QDateTime &recurrenceId) Q_DECL_OVERRIDE;

    /**
      @copydoc Calendar::incidenceUpdated(const QString &,const QDateTime &)
    */
    void incidenceUpdated(const QString &uid, const QDateTime &recurrenceId) Q_DECL_OVERRIDE;

    using QObject::event;   // prevent warning about hidden virtual method

protected:
    /**
      @copydoc IncidenceBase::virtual_hook()
    */
    void virtual_hook(int id, void *data) Q_DECL_OVERRIDE;

private:
    //@cond PRIVATE
    class Private;
    Private *const d;
    //@endcond

    Q_DISABLE_COPY(MemoryCalendar)
};

}

#endif
