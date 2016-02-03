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

#ifndef COURSEINFOMODEL_H
#define COURSEINFOMODEL_H

#include <QTime>

#include <QAbstractTableModel>

class CourseInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    struct CourseInformation
    {
        int day;
        QTime start;
        QTime end;
        QString position;
    };
    explicit CourseInfoModel(QObject *parent = 0);

    void append(int day,
                const QString &start,
                const QString &end,
                const QString &position);
    void append(const QList<CourseInformation> &datas);

    int columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return 4;
    }

    int rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return m_infoList.size();
    }

    QList<CourseInformation> infoList() const;

    void clear();
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QString defaultPosition() const;
    void setDefaultPosition(const QString &defaultPosition);
    static QTime getTime(const QString &data);

signals:
    void rowChanged();

public slots:

private:
    inline QString timeText(const QTime &time) const;
    inline QString getDay(int day) const;
    QList<CourseInformation> m_infoList;
    QString m_defaultPosition;
};

#endif // COURSEINFOMODEL_H
