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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class TimeTableViewer;
class TimeTableModel;
class CourseInfoEditor;
class AddCourseDialog;
class QTreeView;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:

private slots:
    void onActionNew();
    void onActionOpen();
    bool onActionSave();
    bool onActionSaveAs();
    void onActionExport();
    void onActionShowViewer();
    void onActionAddCourse();
    void onActionRemoveCourse();
    void onActionCurrentChanged(const QModelIndex &index);

private:
    inline void addAction(const QString &title,
                          QMenu *menu,
                          const char *method,
                          const QKeySequence &shortcut=QKeySequence());
    TimeTableViewer *m_viewer;
    TimeTableModel *m_model;
    AddCourseDialog *m_dialog;
    CourseInfoEditor *m_editor;
    QTreeView *m_courseList;
    QString m_currentFile;
};

#endif // MAINWINDOW_H
