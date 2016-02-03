/*
 * Copyright (C) Kreogist Dev Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QTime>

#include <QObject>

class TimeTableModel : public QObject
{
    Q_OBJECT
public:
    enum CourseType
    {
        Lecture,
        Tutorial,
        Lab
    };

    struct CourseTime
    {
        int day;
        QTime start;
        QTime end;
        QString position;
    };

    struct Course
    {
        QString code;
        int type;
        QString position;
        QList<CourseTime> timeTable;
    };

    explicit TimeTableModel(QObject *parent = 0);

    void appendCourse(const QString &code,
                      int type,
                      const QString &position,
                      const QList<CourseTime> &timeTable);

    void reset();

    int courseCount() const
    {
        return m_courses.size();
    }

    int selectedCourse() const
    {
        return m_selectedRow;
    }

    Course course(int i)
    {
        return m_courses.at(i);
    }

    QByteArray toJson();

    void fromJson(const QByteArray &json);

    void clearChangeFlag()
    {
        m_changed=false;
    }

    bool isChanged() const
    {
        return m_changed;
    }

    void updateItem(int i, const Course &course);
    void remove(int i);

signals:
    void modelChange();
    void selectedChange();
    void appendRow(int i);

public slots:
    void select(int rowIndex);
    void clearSelect();
    void resetChanged()
    {
        m_changed=false;
    }

private:
    QList<Course> m_courses;
    int m_selectedRow;
    bool m_changed;
};

#endif // TIMETABLEMODEL_H
