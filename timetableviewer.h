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

#ifndef TIMETABLEVIEWER_H
#define TIMETABLEVIEWER_H

#include <QWidget>

class TimeTableModel;
class TimeTableViewer : public QWidget
{
    Q_OBJECT
public:
    explicit TimeTableViewer(QWidget *parent = 0);

    TimeTableModel *model() const;
    void setModel(TimeTableModel *model);

    QPixmap timeTable();

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private slots:
    void onActionModelChanged();

private:
    void updateTimeTable();
    void drawTimeTable(QPixmap &pixmap, bool highLight);
    QStringList m_times, m_apm, m_workingDays;
    TimeTableModel *m_model;
    QColor m_timeColor[3];
    QString m_type[3];
    QColor m_selectColor;
    QPixmap m_timetable;
};

#endif // TIMETABLEVIEWER_H
