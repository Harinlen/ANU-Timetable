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
#include "timetablemodel.h"

#include "courselistmodel.h"

CourseListModel::CourseListModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_model(nullptr)
{

}

int CourseListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return (m_model==nullptr)?0:m_model->courseCount();
}

int CourseListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant CourseListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical)
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        switch(section)
        {
        case 0:
            return "Course ID";
        case 1:
            return "Type";
        case 2:
            return "Class Count";
        }
    }
    return QVariant();
}

QVariant CourseListModel::data(const QModelIndex &index, int role) const
{
    if(m_model==nullptr || (!index.isValid()))
    {
        return QVariant();
    }
    //Get the course object.
    TimeTableModel::Course course=m_model->course(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case 0:
            return course.code;
        case 1:
            return courseType(course.type);
        case 2:
            return QString::number(course.timeTable.size());
        }
    default:
        break;
    }
    return QVariant();
}

TimeTableModel *CourseListModel::timeTableModel() const
{
    return m_model;
}

void CourseListModel::setTimeTableModel(TimeTableModel *model)
{
    m_model = model;
    connect(m_model, SIGNAL(appendRow(int)), this, SLOT(onActionAppendRow(int)));
}

void CourseListModel::onActionAppendRow(int row)
{
    beginInsertRows(QModelIndex(), row, row);
    endInsertRows();
}

inline QString CourseListModel::courseType(int type) const
{
    switch(type)
    {
    case 0:
        return "Lecture";
    case 1:
        return "Tutorial";
    case 2:
        return "Lab";
    }
    return "";
}
