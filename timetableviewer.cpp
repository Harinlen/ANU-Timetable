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
#include <QPainter>
#include <QTime>

#include "timetablemodel.h"

#include "timetableviewer.h"

TimeTableViewer::TimeTableViewer(QWidget *parent) :
    QWidget(parent),
    m_model(nullptr),
    m_selectColor(QColor(0xF7, 0xCF, 0x3D)),
    m_timetable(QPixmap())
{
    setMinimumSize(640, 480);
    m_times << "7" << "8" << "9" << "10" << "11" << "12" << "1" << "2" << "3"
            << "4" << "5";
    m_apm << "AM" << "AM" << "AM" << "AM" << "AM" << "AM" << "PM" << "PM"
          << "PM" << "PM" << "PM";
    m_workingDays << "Monday" << "Tuesday" << "Wednesday" << "Thursday"
                  << "Friday";
    m_timeColor[TimeTableModel::Lecture]=QColor(0x7d, 0x30, 0x74);
    m_timeColor[TimeTableModel::Tutorial]=QColor(0xa0, 0x34, 0x25);
    m_timeColor[TimeTableModel::Lab]=QColor(0xa0, 0x34, 0x25);

    m_type[TimeTableModel::Lecture]="Lecture";
    m_type[TimeTableModel::Tutorial]="Tutorial";
    m_type[TimeTableModel::Lab]="Lab";
}

void TimeTableViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.drawPixmap(0,0,m_timetable);
}

void TimeTableViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateTimeTable();
}

void TimeTableViewer::onActionModelChanged()
{
    updateTimeTable();
    update();
}

void TimeTableViewer::updateTimeTable()
{
    drawTimeTable(m_timetable, true);
}

void TimeTableViewer::drawTimeTable(QPixmap &pixmap, bool highLight)
{
    int fontHeight=fontMetrics().height(), headerHeight=fontHeight<<1,
            columnHeight=(height()-headerHeight)/22,
            columnItemHeight=columnHeight<<1;

    //Calculate items.
    QFont timeFont=font();
    timeFont.setBold(true);
    timeFont.setPixelSize(timeFont.pixelSize()<<1);
    QFontMetrics timeFontMetrics(timeFont);
    int columnWidth=-1;
    for(int i=0; i<m_times.size(); ++i)
    {
        columnWidth=qMax(columnWidth,
                         timeFontMetrics.width(m_times.at(i)) +
                         fontMetrics().width(m_apm.at(i)));
    }
    columnWidth+=20;
    int dayWidth=(width()-columnWidth)/5;

    pixmap=QPixmap(width(), columnItemHeight*11+headerHeight);

    QPainter painter(&pixmap);
    QFont tableFont=font();
    tableFont.setFamily("Arial");
    painter.setFont(tableFont);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);
    painter.fillRect(rect(), QColor(0xf9, 0xf9, 0xf9));

    painter.setPen(QColor(0xdd, 0xdd, 0xdd));
    QLinearGradient header(0,0,0,fontHeight);
    header.setColorAt(0, QColor(0x75, 0x75, 0x74));
    header.setColorAt(1, QColor(0x61, 0x61, 0x61));
    painter.setBrush(header);
    painter.drawRect(QRect(0,0,width(),headerHeight));
    painter.drawRect(QRect(0,headerHeight,columnWidth,height()-headerHeight));

    for(int i=0, columnTop=headerHeight; i<m_times.size(); ++i)
    {
        //Draw top and middle lines.
        painter.drawLine(0, columnTop, width(), columnTop);
        painter.drawLine(columnWidth, columnTop+columnHeight,
                         width(),columnTop+columnHeight);
        //Draw time.
        painter.setFont(timeFont);
        painter.drawText(5, columnTop, columnWidth, columnItemHeight,
                         Qt::AlignTop, m_times.at(i));
        painter.setFont(font());
        painter.drawText(timeFontMetrics.width(m_times.at(i))+7,
                         columnTop+3,
                         columnWidth,
                         columnItemHeight,
                         Qt::AlignTop,
                         m_apm.at(i));
        //Move to next.
        columnTop+=columnItemHeight;
    }

    for(int i=0, dayLeft=columnWidth; i<5; ++i)
    {
        painter.setPen(QColor(0xff, 0xff, 0xff));
        //Draw day.
        painter.drawText(dayLeft, 0, dayWidth, headerHeight, Qt::AlignCenter,
                         m_workingDays.at(i));
        //Draw line.
        painter.setPen(QColor(0xdd, 0xdd, 0xdd));
        painter.drawLine(dayLeft, 0, dayLeft, height());
        //Move day left.
        dayLeft += dayWidth;
    }

    //If model is not null.
    if(m_model!=nullptr)
    {
        for(int i=0; i<m_model->courseCount(); ++i)
        {
            //Get course.
            TimeTableModel::Course course=m_model->course(i);
            //Draw course.
            for(int j=0; j<course.timeTable.size(); ++j)
            {
                //Calculate position.
                int startPosition=(course.timeTable.at(j).start.hour()-7)*columnItemHeight
                        +(course.timeTable.at(j).start.minute()/30)*columnHeight,
                    endPosition=(course.timeTable.at(j).end.hour()-7)*columnItemHeight
                        +(course.timeTable.at(j).end.minute()/30)*columnHeight;
                painter.setOpacity(0.7);
                painter.setPen(Qt::NoPen);
                painter.setBrush((i==m_model->selectedCourse() && highLight) ?
                                     m_selectColor:m_timeColor[course.type]);
                QRect courseRect=QRect(
                        course.timeTable.at(j).day*dayWidth+columnWidth,
                        startPosition+headerHeight,
                        dayWidth,
                        endPosition-startPosition);
                painter.drawRoundedRect(courseRect, 3, 3);
                painter.setOpacity(1.0);
                painter.drawRoundedRect(QRect(courseRect.left(),
                                 courseRect.top(),
                                 courseRect.width(),
                                 fontHeight+4), 3, 3);
                painter.setPen(QColor(0xff,0xff,0xff));
                painter.drawText(courseRect.left()+4,
                                 courseRect.top()+2,
                                 courseRect.width(),
                                 fontHeight,
                                 Qt::AlignVCenter,course.code);
                painter.drawText(courseRect.left()+4,
                                 courseRect.top()+6+fontHeight,
                                 courseRect.width(),
                                 courseRect.height(),
                                 Qt::AlignTop,
                                 m_type[course.type] + "\n" +
                                 QString::number(course.timeTable.at(j).start.hour())+":"+
                        (course.timeTable.at(j).start.minute()<10?
                   "0"+QString::number(course.timeTable.at(j).start.minute()):QString::number(course.timeTable.at(j).start.minute())) + " - " +
                                 QString::number(course.timeTable.at(j).end.hour())+":"+
                                 (course.timeTable.at(j).end.minute()<10?
                            "0"+QString::number(course.timeTable.at(j).end.minute()):QString::number(course.timeTable.at(j).end.minute())) + "\n" +
                                (course.timeTable.at(j).position.isEmpty()?
                                    course.position:course.timeTable.at(j).position));
            }
        }
    }
    painter.end();
}

TimeTableModel *TimeTableViewer::model() const
{
    return m_model;
}

void TimeTableViewer::setModel(TimeTableModel *model)
{
    if(m_model!=nullptr)
    {
        disconnect(m_model, &TimeTableModel::modelChange,
                   this, &TimeTableViewer::onActionModelChanged);
        disconnect(m_model, &TimeTableModel::selectedChange,
                   this, &TimeTableViewer::onActionModelChanged);
    }
    m_model = model;
    if(m_model!=nullptr)
    {
        connect(m_model, &TimeTableModel::modelChange,
                this, &TimeTableViewer::onActionModelChanged);
        connect(m_model, &TimeTableModel::selectedChange,
                this, &TimeTableViewer::onActionModelChanged);
    }
    updateTimeTable();
    update();
}

QPixmap TimeTableViewer::timeTable()
{
    QPixmap resultTable;
    drawTimeTable(resultTable, false);
    return resultTable;
}

