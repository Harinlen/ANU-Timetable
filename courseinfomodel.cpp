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


#include "courseinfomodel.h"

CourseInfoModel::CourseInfoModel(QObject *parent) : QAbstractTableModel(parent)
{

}

void CourseInfoModel::append(int day, const QString &start,
                             const QString &end,
                             const QString &position)
{
    CourseInformation info;
    info.day=day;
    info.start=getTime(start);
    info.end=getTime(end);
    if(info.start.hour()<7 || info.start.hour()>17 || info.end.hour()<7 || info.end.hour()>17)
    {
        return;
    }
    if(!position.isEmpty())
    {
        info.position=position;
    }
    beginInsertRows(QModelIndex(), m_infoList.size(), m_infoList.size());
    m_infoList.append(info);
    endInsertRows();
    emit rowChanged();
}

void CourseInfoModel::append(const QList<CourseInfoModel::CourseInformation> &datas)
{
    if(datas.isEmpty())
    {
        return;
    }
    beginInsertRows(QModelIndex(), m_infoList.size(), m_infoList.size()+datas.size()-1);
    m_infoList.append(datas);
    endInsertRows();
    emit rowChanged();
}

QList<CourseInfoModel::CourseInformation> CourseInfoModel::infoList() const
{
    return m_infoList;
}

void CourseInfoModel::clear()
{
    if(m_infoList.isEmpty())
    {
        return;
    }
    m_defaultPosition.clear();
    beginRemoveRows(QModelIndex(), 0, m_infoList.size()-1);
    m_infoList.clear();
    endRemoveRows();
    emit rowChanged();
}

Qt::ItemFlags CourseInfoModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QVariant CourseInfoModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    if(role==Qt::DisplayRole || role==Qt::EditRole)
    {
        const CourseInformation &info=m_infoList.at(index.row());
        switch(index.column())
        {
        case 0:
            return role==Qt::DisplayRole?
                        getDay(info.day):QString::number(info.day);
        case 1:
            return timeText(info.start);
        case 2:
            return timeText(info.end);
        case 3:
            return (info.position.isEmpty()?
                        m_defaultPosition:
                        info.position);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool CourseInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
    {
        return false;
    }
    if(role==Qt::DisplayRole || role==Qt::EditRole)
    {
        CourseInformation info=m_infoList.at(index.row());
        switch(index.column())
        {
        case 0:
        {
            int day=value.toString().toInt();
            if(day<0 || day>4)
            {
                return false;
            }
            info.day=day;
            break;
        }
        case 1:
        {
            QTime nt=getTime(value.toString());
            if(nt>=info.end)
            {
                return false;
            }
            info.start=nt;
            break;
        }
        case 2:
        {
            QTime nt=getTime(value.toString());
            if(nt<=info.start)
            {
                return false;
            }
            info.end=nt;
            break;
        }
        case 3:
            info.position=value.toString();
            break;
        default:
            break;
        }
        m_infoList.replace(index.row(), info);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool CourseInfoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent)
    beginRemoveRows(QModelIndex(), row, row+count-1);
    while(count--)
    {
        m_infoList.removeAt(row);
    }
    endRemoveRows();
    emit rowChanged();
    return true;
}

QVariant CourseInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical)
    {
        return QVariant();
    }
    if(role==Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return "Weekday";
        case 1:
            return "Start Time";
        case 2:
            return "End Time";
        case 3:
            return "Position";
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QString CourseInfoModel::defaultPosition() const
{
    return m_defaultPosition;
}

void CourseInfoModel::setDefaultPosition(const QString &defaultPosition)
{
    m_defaultPosition = defaultPosition;
}

QTime CourseInfoModel::getTime(const QString &data)
{
    int colonPos=data.indexOf(':');
    if(colonPos==-1)
    {
        int hourData=data.toInt();
        return QTime(hourData<7?hourData+12:hourData, 0);
    }
    int hourData=data.left(colonPos).toInt();
    return QTime(hourData<7?hourData+12:hourData, data.mid(colonPos+1).toInt());
}

inline QString CourseInfoModel::timeText(const QTime &time) const
{
    return QString::number(time.hour()) + ":" +
            (time.minute()<10?
                "0"+QString::number(time.minute()):
                 QString::number(time.minute()));
}

inline QString CourseInfoModel::getDay(int day) const
{
    switch(day)
    {
    case 0:
        return "Monday";
    case 1:
        return "Tuesday";
    case 2:
        return "Wednesday";
    case 3:
        return "Thursday";
    case 4:
        return "Friday";
    }
}
