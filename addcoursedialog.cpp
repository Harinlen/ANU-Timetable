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
#include <QPushButton>
#include <QBoxLayout>

#include "courseinfoeditor.h"
#include "timetablemodel.h"

#include "addcoursedialog.h"

AddCourseDialog::AddCourseDialog(QWidget *parent) :
    QWidget(parent),
    m_okay(new QPushButton("Ok", this)),
    m_cancel(new QPushButton("Cancel", this)),
    m_editor(new CourseInfoEditor(this)),
    m_model(nullptr)
{
    QBoxLayout *layout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->addWidget(m_editor, 1);
    QBoxLayout *buttonLayout=new QBoxLayout(QBoxLayout::LeftToRight, layout->widget());
    layout->addLayout(buttonLayout);
    buttonLayout->addWidget(m_okay);
    buttonLayout->addWidget(m_cancel);
    connect(m_okay, SIGNAL(clicked(bool)), this, SLOT(onActionOk()));
    connect(m_cancel, SIGNAL(clicked(bool)), this, SLOT(onActionCancel()));

    m_okay->setDefault(true);
}

void AddCourseDialog::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_editor->reset();
}

void AddCourseDialog::onActionOk()
{
    QList<CourseInfoModel::CourseInformation>
            infoList=m_editor->infoList();
    QList<TimeTableModel::CourseTime> courseTime;
    for(int i=0; i<infoList.size(); ++i)
    {
        TimeTableModel::CourseTime ct;
        ct.day=infoList.at(i).day;
        ct.position=infoList.at(i).position;
        ct.start=infoList.at(i).start;
        ct.end=infoList.at(i).end;
        courseTime.append(ct);
    }
    m_model->appendCourse(m_editor->id(),
                          m_editor->type(),
                          m_editor->position(),
                          courseTime);
    close();
}

void AddCourseDialog::onActionCancel()
{
    close();
}

TimeTableModel *AddCourseDialog::model() const
{
    return m_model;
}

void AddCourseDialog::setModel(TimeTableModel *model)
{
    m_model = model;
}

