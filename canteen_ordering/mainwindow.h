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

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

struct Dish
{
    QString name;
    double price;
    QString desc;
    QString category;
};

struct Order
{
    QString name;
    double price;
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

    QList<Dish> dishes;   // 所有菜品
    QList<Order> orders;  // 当前购物车订单

    // 菜品文件操作
    void loadDishes();
    void saveDishes();

    // 刷新界面
    void refreshMenu();
    void refreshCart();
    void refreshAdminOrders();

   private slots:
    // 顾客操作
    void onAddToCart();
    void onRemoveFromCart();
    void onCheckout();

    // 管理员操作
    void onAdminAddDish();
    void onAdminDeleteDish();
    void onAdminRemoveOrder();

    // 管理员登录/退出
    void onAdminLogin();
    void onAdminLogout();
};

#endif  // MAINWINDOW_H
