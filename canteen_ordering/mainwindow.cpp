#include "mainwindow.h"

#include <QInputDialog>
#include <QMessageBox>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 默认显示“用户界面”
    ui->stackedWidget->setCurrentIndex(0);

    // 管理员登录
    connect(ui->actionlog, &QAction::triggered, this,
            [=]()
            {
                bool ok;
                QString password = QInputDialog::getText(
                    this, tr("管理员登录"), tr("请输入管理员密码："), QLineEdit::Password, "", &ok);
                if (ok && !password.isEmpty())
                {
                    if (password == "123456")
                    {  // ✅ 修改这里的密码
                        QMessageBox::information(this, "登录成功", "欢迎进入管理员界面！");
                        ui->stackedWidget->setCurrentIndex(1);  // 切换到管理员界面
                    }
                    else
                    {
                        QMessageBox::warning(this, "错误", "密码错误！");
                    }
                }
            });

    // 管理员退出
    connect(ui->actionexit, &QAction::triggered, this,
            [=]()
            {
                ui->stackedWidget->setCurrentIndex(0);  // 回到用户界面
                QMessageBox::information(this, "提示", "已退出管理员模式");
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}
