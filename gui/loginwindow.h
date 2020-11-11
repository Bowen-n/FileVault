#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLineEdit>

#include "mainwindow.h"
#include "utils.h"
#include "user.h"
#include "comm.h"

class LoginWindow: public QWidget
{
    Q_OBJECT

private:
    // widgets
    QLineEdit *password_edit;
    QLabel *ps_label;
    QLabel *prompt_label;
    QLabel *status_label;
    QPushButton *confirm_button;

    // layouts
    QVBoxLayout *global_layout;
    QHBoxLayout *pswd_layout;
    
    // modules
    MainWindow *mainwindow;
    KernelComm *netlinker;

    // vars
    bool login_before;
    User *user;

public slots:
    void parsePassword();

public:
    LoginWindow(QWidget *parent=0);
    void setLayouts();
    ~LoginWindow();

    // logic
    bool loginBefore();

};

#endif