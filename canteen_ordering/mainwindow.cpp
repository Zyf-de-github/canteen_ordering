
#define PASSWORD "123456"

#include "mainwindow.h"

#include <QDialog>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QVBoxLayout>

#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 默认显示用户界面
    ui->stackedWidget->setCurrentIndex(0);

    // 初始化菜单 tab 对应 widget
    menuWidgets = {{"盖浇饭", ui->listWidget_0}, {"凉菜", ui->listWidget_1},
                   {"荤菜", ui->listWidget_2},   {"素菜", ui->listWidget_3},
                   {"甜点", ui->listWidget_4},   {"饮品", ui->listWidget_5},
                   {"其他", ui->listWidget_6}};

    adminWidgets = {{"盖浇饭", ui->listWidget_7}, {"凉菜", ui->listWidget_8},
                    {"荤菜", ui->listWidget_9},   {"素菜", ui->listWidget_10},
                    {"甜点", ui->listWidget_11},  {"饮品", ui->listWidget_12},
                    {"其他", ui->listWidget_13}};

    //重置订单数
    orderNumbers=1;

    // 加载菜品文件
    loadDishes();
    refreshMenu();

    // 顾客界面按钮
    connect(ui->pushButton_0, &QPushButton::clicked, this, &MainWindow::onAddToShopping);
    connect(ui->pushButton_1, &QPushButton::clicked, this, &MainWindow::onRemoveFromShopping);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onCheckout);

    // 管理员界面按钮
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onAdminAddDish);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::onAdminDeleteDish);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::onAdminRemoveOrder);

    // 管理员菜单
    connect(ui->actionlog, &QAction::triggered, this, &MainWindow::onAdminLogin);
    connect(ui->actionexit, &QAction::triggered, this, &MainWindow::onAdminLogout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------- 菜品文件操作 --------------------
//从文件中加载菜单
void MainWindow::loadDishes()
{
    dishes.clear();
    QFile file("dishes.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList parts = line.split("\t");
        if (parts.size() == 4)
        {
            Dish d;
            d.name = parts[0];
            d.price = parts[1].toDouble();
            d.desc = parts[2];
            d.category = parts[3];
            dishes.append(d);
        }
    }
    file.close();
}

//修改菜单后保存菜单
void MainWindow::saveDishes()
{
    QFile file("dishes.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    for (const Dish& d : dishes)
    {
        out << d.name << "\t" << d.price << "\t" << d.desc << "\t" << d.category << "\n";
    }
    file.close();
}

// -------------------- 刷新界面 --------------------
//刷新菜单界面
void MainWindow::refreshMenu()
{
    for (auto w : menuWidgets) w->clear();
    for (auto w : adminWidgets) w->clear();

    for (const Dish& d : dishes)
    {
        if (menuWidgets.contains(d.category))
            menuWidgets[d.category]->addItem(d.name + "  ¥" + QString::number(d.price, 'f', 2));
        if (adminWidgets.contains(d.category))
            adminWidgets[d.category]->addItem(d.name + "  ¥" + QString::number(d.price, 'f', 2) +
                                              "  " + d.desc);
    }
}

//刷新购物车菜品
void MainWindow::refreshShopping()
{
    ui->listWidget_a->clear();
    double total = 0;
    for (const Shopping& S : shoppings)
    {
        ui->listWidget_a->addItem(S.name + "  ¥" + QString::number(S.price, 'f', 2));
        total += S.price;
    }
    ui->lineEdit->setText(QString::number(total, 'f', 2));
}

//刷新后台订单
void MainWindow::refreshAdminOrders()
{
    ui->listWidget_b->clear();
    for (const Order& o : orders)
    {
        ui->listWidget_b->addItem(o.name + "  订单号：" + QString::number(o.times));
    }
}

// -------------------- 顾客操作 --------------------
void MainWindow::onAddToShopping()
{
    QWidget* currentTab = ui->tabWidget->currentWidget();
    QListWidget* list = currentTab->findChild<QListWidget*>();
    if (!list) return;

    QListWidgetItem* item = list->currentItem();
    if (!item) return;

    QString text = item->text();
    QString name = text.section("  ¥", 0, 0);
    double price = text.section("  ¥", 1, 1).toDouble();

    shoppings.append({name, price});
    orders.append({name,orderNumbers});
    refreshShopping();
}

void MainWindow::onRemoveFromShopping()
{
    QListWidgetItem* item = ui->listWidget_a->currentItem();
    if (!item) return;

    QString text = item->text();
    QString name = text.section("  ¥", 0, 0);

    for (int i = 0; i < shoppings.size(); ++i)
    {
        if (shoppings[i].name == name)
        {
            shoppings.remove(i);
            break;
        }
    }
    for (int i = 0; i < orders.size(); ++i)
    {
        if (orders[i].name == name)
        {
            orders.remove(i);
            break;
        }
    }
    refreshShopping();
}

void MainWindow::onCheckout()
{
    if (shoppings.isEmpty()) return;

    refreshAdminOrders();
    shoppings.clear();
    refreshShopping();
    orderNumbers++;
}

// -------------------- 管理员操作 --------------------
void MainWindow::onAdminAddDish()
{
    bool ok;
    QString name = QInputDialog::getText(this, "添加菜品", "名称", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    double price = QInputDialog::getDouble(this, "添加菜品", "价格", 0, 0, 10000, 2, &ok);
    if (!ok) return;

    QString desc = QInputDialog::getText(this, "添加菜品", "描述", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QStringList categories = {"盖浇饭", "凉菜", "荤菜", "素菜", "甜点", "饮品", "其他"};
    QString category =
        QInputDialog::getItem(this, "添加菜品", "选择分类", categories, 0, false, &ok);
    if (!ok) return;

    dishes.append({name, price, desc, category});
    saveDishes();
    refreshMenu();
}

void MainWindow::onAdminDeleteDish()
{
    QWidget* currentTab = ui->tabWidget_2->currentWidget();
    QListWidget* list = currentTab->findChild<QListWidget*>();
    if (!list) return;

    QListWidgetItem* item = list->currentItem();
    if (!item) return;

    QString text = item->text();
    QString name = text.section("  ¥", 0, 0);

    for (int i = 0; i < dishes.size(); ++i)
    {
        if (dishes[i].name == name)
        {
            dishes.remove(i);
            break;
        }
    }
    saveDishes();
    refreshMenu();
}

void MainWindow::onAdminRemoveOrder()
{
    QListWidgetItem* item = ui->listWidget_b->currentItem();
    if (!item) return;

    QString text = item->text();
    QString name = text.section("  订单号：", 0, 0);

    for (int i = 0; i < orders.size(); ++i)
    {
        if (orders[i].name == name)
        {
            orders.remove(i);
            break;
        }
    }
    refreshAdminOrders();
}

// -------------------- 管理员登录/退出 --------------------
void MainWindow::onAdminLogin()
{
    bool ok;
    QString password = QInputDialog::getText(this, "管理员登录", "请输入密码", QLineEdit::Password, "", &ok);
    if (!ok) return;

    if (password == "123456")
    {
        ui->stackedWidget->setCurrentIndex(1);
        QMessageBox::information(this, "登录成功", "管理员登录成功");
    }
    else
    {
        QMessageBox::warning(this, "登录失败", "密码错误");
    }
}

void MainWindow::onAdminLogout()
{
    ui->stackedWidget->setCurrentIndex(0);
    QMessageBox::information(this, "退出登录", "已退出管理员界面");
}
