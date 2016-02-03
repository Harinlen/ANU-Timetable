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

#ifndef COURSEINFOEDITOR_H
#define COURSEINFOEDITOR_H

#include "knconnectionhandler.h"
#include "courseinfomodel.h"

#include <QWidget>

class QPushButton;
class QTreeView;
class QComboBox;
class QLineEdit;
class TimeTableModel;
class CourseInfoEditor : public QWidget
{
    Q_OBJECT
public:
    explicit CourseInfoEditor(QWidget *parent = 0);
    void reset();
    QString id() const;
    QString position() const;
    int type() const;
    QList<CourseInfoModel::CourseInformation> infoList() const;
    void displayData(const QString &code,
                     int type,
                     const QString &position,
                     const QList<CourseInfoModel::CourseInformation> &timeTable);

    void setCurrentIndex(int currentIndex);

    void setRawModel(TimeTableModel *rawModel);

signals:

public slots:

private slots:
    void onActionAdd();
    void onActionRemove();
    void onActionTextChange();

    void idChange(const QString &text);
    void positionChange(const QString &position);
    void typeChange(int type);
    void coursePositionChange();

private:
    KNConnectionHandler m_handler;
    QLineEdit *m_id, *m_position, *m_start, *m_end, *m_coursePosition;
    QComboBox *m_type, *m_days;
    QTreeView *m_courseInfo;
    CourseInfoModel *m_model;
    TimeTableModel *m_rawModel;
    QPushButton *m_add, *m_remove;
    int m_currentIndex;
};

#endif // COURSEINFOEDITOR_H
