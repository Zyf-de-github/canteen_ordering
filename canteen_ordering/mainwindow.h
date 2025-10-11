#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QtCharts>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

// 菜品数据结构
struct Dish
{
    QString name;      // 菜名
    double price;      // 价格
    QString desc;      // 描述
    QString category;  // 菜品种类
};

// 购物车数据结构
struct Shopping
{
    QString name;  // 菜名
    double price;  // 价格
};

// 订单数据结构
struct Order
{
    QString name;  // 菜名
    int times;     // 订单号
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow* ui;

    QMap<QString, QListWidget*> menuWidgets;   // 顾客界面菜单
    QMap<QString, QListWidget*> adminWidgets;  // 管理员界面菜单

    QList<Dish> dishes;         // 所以菜品容器
    QList<Shopping> shoppings;  // 购物车容器
    QList<Order> orders;        // 订单容器

    // 统计数据
    int orderNumbers;  // 订单号
    int totalEarn;     // 总收入

    // 菜品文件操作
    void loadDishes();
    void saveDishes();

    // 刷新界面
    void refreshMenu();
    void refreshShopping();
    void refreshAdminOrders();  // 更新购物车

   private slots:
    // 顾客操作
    void onAddToShopping();
    void onRemoveFromShopping();
    void onCheckout();

    // 管理员操作
    void onAdminAddDish();
    void onAdminDeleteDish();
    void onAdminRemoveOrder();

    // 页面切换
    void onAdminAdmin();
    void onAdminClient();
    void onAdminAnalysis();

    //绘图分析
    void setupCharts();
};

#endif  // MAINWINDOW_H
