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
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QTreeView>

#include "timetablemodel.h"

#include "courseinfoeditor.h"

CourseInfoEditor::CourseInfoEditor(QWidget *parent) : QWidget(parent),
    m_id(new QLineEdit(this)),
    m_position(new QLineEdit(this)),
    m_start(new QLineEdit(this)),
    m_end(new QLineEdit(this)),
    m_coursePosition(new QLineEdit(this)),
    m_type(new QComboBox(this)),
    m_days(new QComboBox(this)),
    m_courseInfo(new QTreeView(this)),
    m_model(new CourseInfoModel(this)),
    m_rawModel(nullptr),
    m_add(new QPushButton(this)),
    m_remove(new QPushButton(this)),
    m_currentIndex(-1)
{
    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    QFormLayout *layout=new QFormLayout(mainLayout->widget());
    mainLayout->addLayout(layout);
    layout->addRow("Course ID", m_id);
    layout->addRow("Position", m_position);
    layout->addRow("Course Type", m_type);
    m_type->addItem("Lecture");
    m_type->addItem("Tutorial");
    m_type->addItem("Lab");

    m_days->addItem("Monday");
    m_days->addItem("Tuesday");
    m_days->addItem("Wednesday");
    m_days->addItem("Thursday");
    m_days->addItem("Friday");

    QWidget *container=new QWidget(this);
    mainLayout->addWidget(container, 1);
    QBoxLayout *containerLayout=new QBoxLayout(QBoxLayout::TopToBottom,
                                               container);
    container->setLayout(containerLayout);
    m_courseInfo->setModel(m_model);
    containerLayout->addWidget(m_courseInfo, 1);
    QBoxLayout *operateLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                             container);
    operateLayout->addWidget(m_days, 1);
    operateLayout->addWidget(m_start, 1);
    operateLayout->addWidget(new QLabel("-", this));
    operateLayout->addWidget(m_end, 1);
    operateLayout->addWidget(new QLabel("Room"));
    operateLayout->addWidget(m_coursePosition, 2);
    operateLayout->addWidget(m_add);
    operateLayout->addWidget(m_remove);
    m_add->setText("+");
    m_remove->setText("-");
    int buttonSize=qMin(m_add->width(), m_add->height())/3*2;
    m_add->setFixedSize(buttonSize,buttonSize);
    m_remove->setFixedSize(buttonSize, buttonSize);
    connect(m_add, SIGNAL(clicked(bool)), this, SLOT(onActionAdd()));
    connect(m_remove, SIGNAL(clicked(bool)), this, SLOT(onActionRemove()));
    containerLayout->addLayout(operateLayout);
    setTabOrder(m_coursePosition, m_add);
    setTabOrder(m_add, m_remove);
    setTabOrder(m_position, m_type);
    setTabOrder(m_type, m_days);
}

void CourseInfoEditor::reset()
{
    m_currentIndex=-1;
    m_id->clear();
    m_position->clear();
    m_start->clear();
    m_end->clear();
    m_coursePosition->clear();
    m_type->setCurrentIndex(0);
    m_days->setCurrentIndex(0);
    m_model->clear();
    m_id->setFocus();
}

QString CourseInfoEditor::id() const
{
    return m_id->text();
}

QString CourseInfoEditor::position() const
{
    return m_position->text();
}

int CourseInfoEditor::type() const
{
    return m_type->currentIndex();
}

QList<CourseInfoModel::CourseInformation> CourseInfoEditor::infoList() const
{
    return m_model->infoList();
}

void CourseInfoEditor::onActionAdd()
{
    if(m_start->text().isEmpty())
    {
        m_start->setFocus();
        return;
    }
    if(m_end->text().isEmpty())
    {
        m_end->setFocus();
        return;
    }
    if(CourseInfoModel::getTime(m_start->text()) >= CourseInfoModel::getTime(m_end->text()))
    {
        m_start->setFocus();
        return;
    }
    m_model->append(m_days->currentIndex(),
                    m_start->text(),
                    m_end->text(),
                    m_coursePosition->text());
    //Clear
    m_days->setCurrentIndex(0);
    m_start->clear();
    m_end->clear();
    m_coursePosition->clear();
    //Reset focus.
    m_days->setFocus();
}

void CourseInfoEditor::onActionRemove()
{
    if(m_courseInfo->currentIndex().isValid())
    {
        m_model->removeRow(m_courseInfo->currentIndex().row());
    }
}

void CourseInfoEditor::onActionTextChange()
{
    m_model->setDefaultPosition(m_position->text());
    m_courseInfo->hide();
    m_courseInfo->show();
}

void CourseInfoEditor::idChange(const QString &text)
{
    if(m_rawModel!=nullptr && m_currentIndex!=-1)
    {
        TimeTableModel::Course course=m_rawModel->course(m_currentIndex);
        course.code=text;
        m_rawModel->updateItem(m_currentIndex, course);
    }
}

void CourseInfoEditor::positionChange(const QString &position)
{
    if(m_rawModel!=nullptr && m_currentIndex!=-1)
    {
        TimeTableModel::Course course=m_rawModel->course(m_currentIndex);
        course.position=position;
        m_rawModel->updateItem(m_currentIndex, course);
    }
}

void CourseInfoEditor::typeChange(int type)
{
    if(m_rawModel!=nullptr && m_currentIndex!=-1)
    {
        TimeTableModel::Course course=m_rawModel->course(m_currentIndex);
        course.type=type;
        m_rawModel->updateItem(m_currentIndex, course);
    }
}

void CourseInfoEditor::coursePositionChange()
{
    if(m_rawModel!=nullptr && m_currentIndex!=-1)
    {
        TimeTableModel::Course course=m_rawModel->course(m_currentIndex);
        QList<CourseInfoModel::CourseInformation> myInfoList=
                m_model->infoList();
        QList<TimeTableModel::CourseTime> courseTime;
        for(int i=0; i<myInfoList.size(); ++i)
        {
            TimeTableModel::CourseTime ct;
            ct.day=myInfoList.at(i).day;
            ct.position=myInfoList.at(i).position;
            ct.start=myInfoList.at(i).start;
            ct.end=myInfoList.at(i).end;
            courseTime.append(ct);
        }
        course.timeTable=courseTime;
        m_rawModel->updateItem(m_currentIndex, course);
    }
}

void CourseInfoEditor::setRawModel(TimeTableModel *rawModel)
{
    m_rawModel = rawModel;
}

void CourseInfoEditor::setCurrentIndex(int currentIndex)
{
    m_currentIndex = currentIndex;
}

void CourseInfoEditor::displayData(const QString &code,
                                   int type,
                                   const QString &position,
                                   const QList<CourseInfoModel::CourseInformation> &timeTable)
{

    m_handler.disconnectAll();

    m_id->setText(code);
    m_type->setCurrentIndex(type);
    m_position->setText(position);
    m_model->clear();
    m_model->append(timeTable);
    m_courseInfo->hide();
    m_courseInfo->show();

    m_handler.append(connect(m_id, SIGNAL(textChanged(QString)), this, SLOT(idChange(QString))));
    m_handler.append(connect(m_position, SIGNAL(textChanged(QString)), this, SLOT(positionChange(QString))));
    m_handler.append(connect(m_position, SIGNAL(textChanged(QString)), this, SLOT(onActionTextChange())));
    m_handler.append(connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChange(int))));
    m_handler.append(connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(coursePositionChange())));
    m_handler.append(connect(m_model, SIGNAL(rowChanged()),this, SLOT(coursePositionChange())));
}

