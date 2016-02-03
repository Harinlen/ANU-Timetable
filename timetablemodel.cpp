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
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "stdlib.h"

#include "timetablemodel.h"

TimeTableModel::TimeTableModel(QObject *parent) :
    QObject(parent),
    m_selectedRow(-1),
    m_changed(false)
{
    ;
}

void TimeTableModel::appendCourse(const QString &code,
                                  int type,
                                  const QString &position,
                                  const QList<CourseTime> &timeTable)
{
    Course cc;
    cc.code=code;
    cc.type=type;
    cc.position=position;
    cc.timeTable=timeTable;
    //Append course.
    m_courses.append(cc);
    emit modelChange();
    emit appendRow(m_courses.size()-1);
    m_changed=true;
}

void TimeTableModel::reset()
{
    m_courses.clear();
    m_changed=false;
    m_selectedRow=-1;
    emit modelChange();
}

QByteArray TimeTableModel::toJson()
{
    QJsonArray courses;
    for(int i=0; i<m_courses.size(); ++i)
    {
        QJsonObject course;
        course.insert("code", m_courses.at(i).code);
        course.insert("type", m_courses.at(i).type);
        course.insert("position", m_courses.at(i).position);
        QJsonArray courseTime;
        for(int j=0; j<m_courses.at(i).timeTable.size(); ++j)
        {
            QJsonObject time;
            time.insert("day", m_courses.at(i).timeTable.at(j).day);
            time.insert("position", m_courses.at(i).timeTable.at(j).position);
            time.insert("start-hour", m_courses.at(i).timeTable.at(j).start.hour());
            time.insert("start-min", m_courses.at(i).timeTable.at(j).start.minute());
            time.insert("end-hour", m_courses.at(i).timeTable.at(j).end.hour());
            time.insert("end-min", m_courses.at(i).timeTable.at(j).end.minute());
            courseTime.append(time);
        }
        course.insert("timeTable", courseTime);
        courses.append(course);
    }
    return QJsonDocument(courses).toJson(QJsonDocument::Compact);
}

void TimeTableModel::fromJson(const QByteArray &json)
{
    m_changed=false;
    m_courses.clear();
    QJsonArray courses=QJsonDocument::fromJson(json).array();
    for(int i=0; i<courses.size(); ++i)
    {
        QJsonObject ccourse=courses.at(i).toObject();
        Course course;
        course.code=ccourse.value("code").toString();
        course.type=ccourse.value("type").toInt();
        course.position=ccourse.value("position").toString();
        QJsonArray courseTime=ccourse.value("timeTable").toArray();
        QList<CourseTime> ccourseTime;
        for(int j=0; j<courseTime.size(); ++j)
        {
            QJsonObject time=courseTime.at(j).toObject();
            CourseTime ctime;
            ctime.day=time.value("day").toInt();
            ctime.position=time.value("position").toString();
            ctime.start=QTime(time.value("start-hour").toInt(),
                              time.value("start-min").toInt());
            ctime.end=QTime(time.value("end-hour").toInt(),
                            time.value("end-min").toInt());
            ccourseTime.append(ctime);
        }
        course.timeTable=ccourseTime;
        m_courses.append(course);
    }
    emit modelChange();
}

void TimeTableModel::updateItem(int i, const TimeTableModel::Course &course)
{
    m_changed=true;
    m_courses.replace(i, course);
    emit modelChange();
}

void TimeTableModel::remove(int i)
{
    m_changed=true;
    m_courses.removeAt(i);
    emit modelChange();
}

void TimeTableModel::select(int rowIndex)
{
    Q_ASSERT(rowIndex>-1 && rowIndex<m_courses.size());
    m_selectedRow=rowIndex;
    emit selectedChange();
}

void TimeTableModel::clearSelect()
{
    m_selectedRow=-1;
    emit selectedChange();
}

