
#define PASSWORD "123456"  // 登录密码

#include "mainwindow.h"

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

    // 重置订单数
    orderNumbers = 1;
    totalEarn = 0;

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
    connect(ui->actionadmin, &QAction::triggered, this, &MainWindow::onAdminAdmin);
    connect(ui->actionanalysis, &QAction::triggered, this, &MainWindow::onAdminAnalysis);
    connect(ui->actionclient, &QAction::triggered, this, &MainWindow::onAdminClient);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------------------- 菜单加载 --------------------
// 从文件中加载菜单
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

// 修改菜单后保存菜单
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

// -------------------- 界面绘制 --------------------
// 刷新菜单界面
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

// 刷新购物车菜品
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

// 刷新后台订单
void MainWindow::refreshAdminOrders()
{
    ui->listWidget_b->clear();
    for (const Order& o : orders)
    {
        ui->listWidget_b->addItem(o.name + "  订单号：" + QString::number(o.times));
    }
}

// -------------------- 顾客操作 --------------------
// 添加菜品到购物车
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
    orders.append({name, orderNumbers});
    refreshShopping();
}

// 从购物车移除菜品
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

// 结算账单
void MainWindow::onCheckout()
{
    if (shoppings.isEmpty()) return;
    double total = 0;
    for (const Shopping& S : shoppings)
    {
        ui->listWidget_a->addItem(S.name + "  ¥" + QString::number(S.price, 'f', 2));
        total += S.price;
    }
    totalEarn += total;
    ui->lineEdit_3->setText(QString::number(totalEarn, 'f', 2));
    ui->lineEdit_2->setText(QString::number(orderNumbers));
    refreshAdminOrders();
    shoppings.clear();
    refreshShopping();
    orderNumbers++;
}

// -------------------- 后台操作 --------------------
// 添加菜品
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

// 删除菜品
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

// 删除订单
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

// -------------------- 数据分析 --------------------
void MainWindow::setupCharts()
{
    // 热门菜品
    QPieSeries* pieSeries = new QPieSeries();
    pieSeries->append("红烧肉", 35);
    pieSeries->append("宫保鸡丁", 25);
    pieSeries->append("拍黄瓜", 15);
    pieSeries->append("米饭", 25);
    pieSeries->setLabelsVisible(true);

    QChart* pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("热门菜品占比");

    QChartView* pieView = new QChartView(pieChart);
    pieView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* layoutPie = new QVBoxLayout(ui->widget_hotDish);
    layoutPie->addWidget(pieView);
    layoutPie->setContentsMargins(0, 0, 0, 0);

    // 热门时间段
    QBarSet* set = new QBarSet("下单数");
    *set << 2 << 5 << 8 << 10 << 12 << 6 << 3 << 1;

    QBarSeries* barSeries = new QBarSeries();
    barSeries->append(set);

    QChart* barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("小时内下单数");

    QStringList hours = {"8点", "9点", "10点", "11点", "12点", "13点", "14点", "15点"};
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(hours);
    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("下单数");
    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QChartView* barView = new QChartView(barChart);
    barView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* layoutBar = new QVBoxLayout(ui->widget_hotTime);
    layoutBar->addWidget(barView);
    layoutBar->setContentsMargins(0, 0, 0, 0);

    // 营收折线图
    QLineSeries* lineSeries = new QLineSeries();
    QList<double> revenue = {1200, 1500, 1800, 2000, 2300, 2600, 3100};
    QStringList days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    for (int i = 0; i < revenue.size(); ++i) lineSeries->append(i, revenue[i]);

    QChart* lineChart = new QChart();
    lineChart->addSeries(lineSeries);
    lineChart->setTitle("每日营收趋势");
    lineChart->createDefaultAxes();

    QCategoryAxis* axisX2 = new QCategoryAxis();
    for (int i = 0; i < days.size(); ++i) axisX2->append(days[i], i);
    lineChart->addAxis(axisX2, Qt::AlignBottom);
    lineSeries->attachAxis(axisX2);

    QChartView* lineView = new QChartView(lineChart);
    lineView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout* layoutLine = new QVBoxLayout(ui->widget_revenue);
    layoutLine->addWidget(lineView);
    layoutLine->setContentsMargins(0, 0, 0, 0);
}

// -------------------- 界面切换 --------------------
// 后台登录
void MainWindow::onAdminAdmin()
{
    bool ok;
    QString password =
        QInputDialog::getText(this, "请登录", "请输入密码", QLineEdit::Password, "", &ok);
    if (!ok) return;

    if (password == PASSWORD)
    {
        ui->stackedWidget->setCurrentIndex(1);
        QMessageBox::information(this, "登录成功", "进入程序后台");
    }
    else
    {
        QMessageBox::warning(this, "登录失败", "密码错误");
    }
}

// 数据分析
void MainWindow::onAdminAnalysis()
{
    bool ok;
    QString password =
        QInputDialog::getText(this, "请登录", "请输入密码", QLineEdit::Password, "", &ok);
    if (!ok) return;

    if (password == PASSWORD)
    {
        ui->stackedWidget->setCurrentIndex(2);
        QMessageBox::information(this, "登录成功", "进入数据分析界面");
    }
    else
    {
        QMessageBox::warning(this, "登录失败", "密码错误");
    }
    setupCharts();
}

// 顾客点餐
void MainWindow::onAdminClient()
{
    ui->stackedWidget->setCurrentIndex(0);
    QMessageBox::information(this, "顾客模式", "已进入点餐界面");
}
