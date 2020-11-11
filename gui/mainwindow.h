#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>

#include "user.h"
#include "comm.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stack>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
using namespace std;


class MainWindow: public QWidget
{
    Q_OBJECT

private:
    // widgets
    QTreeWidget *file_tree;
    QPushButton *refresh_button;

    // layouts
    QHBoxLayout *header_layout;
    QVBoxLayout *global_layout;

    // logic
    string current_path;
    string root_path;

    // modules
    User *user;
    KernelComm *netlinker;

    string parseFileSize(size_t);
    string parseFileTime(time_t);
    bool isDirectory(mode_t);

public slots:
    void listDirectory(QTreeWidgetItem*, int);
    void updateFileTree();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setLayouts();
    void show();
   
};

#endif