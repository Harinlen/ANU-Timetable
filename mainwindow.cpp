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
#include <QBoxLayout>
#include <QFile>
#include <QTime>
#include <QMenuBar>
#include <QMessageBox>
#include <QTreeView>
#include <QFileDialog>

#include "timetablemodel.h"
#include "timetableviewer.h"
#include "courseinfoeditor.h"
#include "courselistmodel.h"
#include "addcoursedialog.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_viewer(new TimeTableViewer(this)),
    m_model(new TimeTableModel(this)),
    m_dialog(new AddCourseDialog(this)),
    m_editor(new CourseInfoEditor(this))
{
    //Set font.
    QFont viewerFont;
    viewerFont.setPixelSize(10);
    viewerFont.setFamily("Arial");
    m_viewer->setFont(viewerFont);
    m_viewer->setWindowFlags(Qt::Dialog);
    m_viewer->setModel(m_model);

    m_dialog->setWindowFlags(Qt::Dialog);
    m_dialog->setModel(m_model);

    //Initial menu.
    QMenuBar *bar=new QMenuBar(this);
    setMenuBar(bar);

    QMenu *menu=new QMenu("&File", bar);
    bar->addMenu(menu);

    addAction("&New", menu, SLOT(onActionNew()), QKeySequence::New);
    addAction("&Open", menu, SLOT(onActionOpen()), QKeySequence::Open);
    addAction("&Save", menu, SLOT(onActionSave()), QKeySequence::Save);
    addAction("Save &As", menu, SLOT(onActionSaveAs()), QKeySequence::SaveAs);
    addAction("Export", menu, SLOT(onActionExport()));

    menu=new QMenu("&Edit", bar);
    bar->addMenu(menu);
    addAction("&Add Course", menu, SLOT(onActionAddCourse()));
    addAction("&Remove Course", menu, SLOT(onActionRemoveCourse()));

    menu=new QMenu("&View", bar);
    bar->addMenu(menu);
    addAction("&Time Table Viewer", menu, SLOT(onActionShowViewer()));

    QWidget *container=new QWidget(this);
    setCentralWidget(container);
    QBoxLayout *layout=new QBoxLayout(QBoxLayout::LeftToRight, container);
    container->setLayout(layout);
    m_courseList=new QTreeView(this);
    CourseListModel *courseListModel=new CourseListModel(this);
    courseListModel->setTimeTableModel(m_model);
    m_courseList->setModel(courseListModel);
    layout->addWidget(m_courseList);
    connect(m_courseList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(onActionCurrentChanged(QModelIndex)));
    layout->addWidget(m_editor, 1);

    m_editor->setEnabled(false);
    m_editor->setRawModel(m_model);

    onActionNew();
}

void MainWindow::onActionNew()
{
    //Check model.
    if(m_model->isChanged())
    {
        switch(QMessageBox::question(this,
                                     "Timetable",
                                     "Timetable is changed, save it?",
                                     QMessageBox::Yes,
                                     QMessageBox::No,
                                     QMessageBox::Cancel))
        {
        case QMessageBox::Yes:
            if(!onActionSave())
            {
                return;
            }
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        }
    }
    //Reset current file path.
    m_currentFile.clear();
    m_model->reset();
    m_editor->reset();
    m_editor->setEnabled(false);
    //Reset title.
    setWindowTitle("ANU Time Table - [Untitled]");
}

void MainWindow::onActionOpen()
{
    //Check model.
    if(m_model->isChanged())
    {
        switch(QMessageBox::question(this,
                                     "ANU Timetable",
                                     "Timetable is changed, save it?",
                                     QMessageBox::Yes,
                                     QMessageBox::No,
                                     QMessageBox::Cancel))
        {
        case QMessageBox::Yes:
            if(!onActionSave())
            {
                return;
            }
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        }
    }
    //Open file.
    QString filePath=QFileDialog::getOpenFileName(this,
                                                  "ANU Timetable",
                                                  QString(),
                                                  "ANU Timetable File (*.json)");
    if(filePath.isEmpty())
    {
        return;
    }
    //Or else, load the file.
    QFile timeTableFile(filePath);
    if(!timeTableFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    m_model->fromJson(timeTableFile.readAll());
    timeTableFile.close();
    m_courseList->hide();
    m_courseList->show();
    //Save current file path.
    m_currentFile=filePath;
    //Update title.
    QFileInfo fileInfo(timeTableFile);
    setWindowTitle("ANU Time Table - ["+fileInfo.fileName()+"]");
}

bool MainWindow::onActionSave()
{
    //Check file path.
    if(m_currentFile.isEmpty())
    {
        return onActionSaveAs();
    }
    //Save file content.
    QFile timeTableFile(m_currentFile);
    if(!timeTableFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    timeTableFile.write(m_model->toJson());
    timeTableFile.close();
    m_model->resetChanged();
    return true;
}

bool MainWindow::onActionSaveAs()
{
    QString filePath=QFileDialog::getSaveFileName(this,
                                                     "ANU Timetable",
                                                     QString(),
                                                     "ANU Timetable File (*.json)");
    if(filePath.isEmpty())
    {
        return false;
    }
    //Save file path.
    m_currentFile=filePath;
    //Save file content.
    QFile timeTableFile(m_currentFile);
    if(!timeTableFile.open(QIODevice::WriteOnly))
    {
        return false;
    }
    timeTableFile.write(m_model->toJson());
    timeTableFile.close();
    m_model->resetChanged();
    //Update title.
    QFileInfo fileInfo(timeTableFile);
    setWindowTitle("ANU Time Table - ["+fileInfo.fileName()+"]");
    return true;
}

void MainWindow::onActionExport()
{
    QString exportPath=QFileDialog::getSaveFileName(this,
                                                    "Save Course Timetable Image",
                                                    QString(),
                                                    "PNG format image (*.png)");
    if(exportPath.isEmpty())
    {
        return;
    }
    QPixmap image=m_viewer->timeTable();
    image.save(exportPath, "png");
}

void MainWindow::onActionShowViewer()
{
    m_viewer->show();
}

void MainWindow::onActionAddCourse()
{
    m_dialog->show();
}

void MainWindow::onActionRemoveCourse()
{
    QModelIndex index=m_courseList->currentIndex();
    //Check whether the index is valid.
    if(index.isValid())
    {
        //Remove model index.
        m_model->remove(index.row());
        //Check model is empty or not.
        if(m_model->courseCount()>0)
        {
            m_courseList->setCurrentIndex(m_courseList->model()->index(0, 0));
        }
        //Check whether model is empty.
        else if(m_model->courseCount()==0)
        {
            m_model->reset();
            m_editor->reset();
            m_editor->setEnabled(false);
        }
    }
}

void MainWindow::onActionCurrentChanged(const QModelIndex &index)
{
    //Check whether the index is valid.
    if(index.isValid())
    {
        //Get course and display course.
        TimeTableModel::Course course=m_model->course(index.row());
        QList<CourseInfoModel::CourseInformation> timeTable;
        for(int i=0; i<course.timeTable.size(); ++i)
        {
            CourseInfoModel::CourseInformation ci;
            ci.day=course.timeTable.at(i).day;
            ci.end=course.timeTable.at(i).end;
            ci.start=course.timeTable.at(i).start;
            ci.position=course.timeTable.at(i).position;
            timeTable.append(ci);
        }
        m_editor->setCurrentIndex(index.row());
        m_editor->displayData(course.code,
                              course.type,
                              course.position,
                              timeTable);
        m_model->select(index.row());
        m_editor->setEnabled(true);
        return;
    }
    m_editor->setCurrentIndex(-1);
    m_model->clearSelect();
    m_editor->reset();
    m_editor->setEnabled(false);
}

inline void MainWindow::addAction(const QString &title, QMenu *menu, const char *method, const QKeySequence &shortcut)
{
    QAction *action=new QAction(title, this);
    action->setShortcut(shortcut);
    menu->addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, method);
}

