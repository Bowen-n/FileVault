#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    // display
    setWindowTitle("FileVault");
    setFixedSize(800, 600);
    this->move(250, 50);

    // logic
    user = new User;
    string p_name(user->pname()); 
    root_path = "/home/safebox/" + p_name + "/";
    netlinker = new KernelComm;

    // widgets
    refresh_button = new QPushButton(QString("refresh"), this);
    refresh_button->setFixedSize(80, 30);

    file_tree = new QTreeWidget(this);
    QStringList tree_header;
    tree_header << "Name" << "Size" << "Last Time";
    file_tree->setHeaderLabels(tree_header);
    file_tree->setColumnWidth(0, 200); 
    file_tree->setColumnWidth(1, 150);
    file_tree->setColumnWidth(2, 300);

    // layout
    this->setLayouts();

    // events
    connect(file_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),\
            this, SLOT(listDirectory(QTreeWidgetItem*, int)));
    connect(refresh_button, SIGNAL(released()), this, SLOT(updateFileTree()));
}

void MainWindow::listDirectory(QTreeWidgetItem* item, int)
{
    if (item->child(0) != NULL)
        return;

    // get fullpath
    stack<string> parentnames;
    QTreeWidgetItem *tmp = item;
    string full_path = root_path;
    while(tmp->parent() != NULL)
    {
        parentnames.push(tmp->parent()->text(0).toStdString());
        tmp = tmp->parent();
    }
    while(!parentnames.empty())
    {
        full_path += parentnames.top(); parentnames.pop();
        full_path += "/";
    }

    string item_name = item->text(0).toStdString();
    full_path += item_name;
    
    // check if item is a directory
    struct stat stat_buf; 
    stat(full_path.c_str(), &stat_buf);
    if (!isDirectory(stat_buf.st_mode))
        return;
    full_path += "/";
    

    DIR* dp; struct dirent* ep;

    // traverse directory
    dp = opendir(full_path.c_str());
    if (dp == NULL)
        return;

    while ((ep=readdir(dp))!= NULL)
    {
        if (strcmp(".", ep->d_name) == 0 || strcmp("..", ep->d_name) == 0)
            continue;

        // new item
        QTreeWidgetItem *child_file_node = new QTreeWidgetItem();
        
        // set name
        string file_name(ep->d_name); child_file_node->setText(0, QString::fromStdString(file_name));
        string abs_path = full_path + file_name;

        // set icon
        if (ep->d_type == 4)
        {
            QIcon icon("res/folder.png");
            child_file_node->setIcon(0, icon);
        }
        else if (ep->d_type == 8)
        {
            QIcon icon("res/file.png");
            child_file_node->setIcon(0, icon);
        }

        // get file info
        struct stat stat_buf; 
        stat(abs_path.c_str(), &stat_buf);

        // set size
        child_file_node->setText(1, QString::fromStdString(parseFileSize(stat_buf.st_size)));
        // set time
        child_file_node->setText(2, QString::fromStdString(parseFileTime(stat_buf.st_mtime)));

        item->addChild(child_file_node);
        
    }
    closedir(dp); 

}

void MainWindow::show()
{
    // overload QWidget::show()
    updateFileTree();
    QWidget::show();
}


void MainWindow::updateFileTree()
{
    file_tree->clear();
    netlinker->send_pid();
    DIR* dp; struct dirent* ep;

    // traverse directory
    dp = opendir(root_path.c_str());
    if (dp != NULL)
    {
        while ((ep=readdir(dp))!= NULL)
        {
            if (strcmp(".", ep->d_name) == 0 || strcmp("..", ep->d_name) == 0)
                continue;

            // new item
            QTreeWidgetItem *file_node = new QTreeWidgetItem(file_tree);
            
            // set name
            string file_name(ep->d_name); file_node->setText(0, QString::fromStdString(file_name));
            string full_path = root_path + file_name;

            // set icon
            if (ep->d_type == 4)
            {
                QIcon icon("res/folder.png");
                file_node->setIcon(0, icon);
            }
            else if (ep->d_type == 8)
            {
                QIcon icon("res/file.png");
                file_node->setIcon(0, icon);
            }

            // get file info
            struct stat stat_buf; 
            stat(full_path.c_str(), &stat_buf);

            // set size
            file_node->setText(1, QString::fromStdString(parseFileSize(stat_buf.st_size)));
            // set time
            file_node->setText(2, QString::fromStdString(parseFileTime(stat_buf.st_mtime)));  

        }
        closedir(dp);
    }

}


bool MainWindow::isDirectory(mode_t mode)
{
    int res = mode & S_IFDIR;
    return (res == S_IFDIR) ? true:false;
}

string MainWindow::parseFileSize(size_t size)
{
    float ret_k = size / 1024.0;
    float ret_m = ret_k / 1024.0;
    float ret_g = ret_m / 1024.0;

    string size_ret;
    if (int(ret_g) != 0)
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_g;
        size_ret = stream.str() + "GB";
    }
    else if (int(ret_m) != 0)
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_m;
        size_ret = stream.str() + "MB";
    }
    else
    {
        stringstream stream; stream << fixed << setprecision(2) << ret_k;
        size_ret = stream.str() + "KB";
    }
    return size_ret;

}

string MainWindow::parseFileTime(time_t time)
{
    struct tm time_buf; char time_str[80];
    localtime_r(&time, &time_buf);
    strftime(time_str, sizeof(time_str), "%c", &time_buf);
    string time_ret(time_str);
    return time_ret;
}

void MainWindow::setLayouts()
{
    header_layout = new QHBoxLayout();
    header_layout->addWidget(refresh_button);
    header_layout->addStretch(0);

    global_layout = new QVBoxLayout();
    global_layout->addLayout(header_layout);
    global_layout->addWidget(file_tree);

    this->setLayout(global_layout);
}


MainWindow::~MainWindow()
{
    delete file_tree;
    delete refresh_button;

    delete header_layout;
    delete global_layout;
}

