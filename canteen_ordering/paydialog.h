#ifndef PAYDIALOG_H
#define PAYDIALOG_H

#include <QDialog>

namespace Ui
{
class PayDialog;
}

class PayDialog : public QDialog
{
    Q_OBJECT

   public:
    explicit PayDialog(QWidget* parent = nullptr, double totalMoney = 0.0);
    ~PayDialog();

   private:
    Ui::PayDialog* ui;

    int remainingSeconds;  // 剩余秒数
    QTimer* timer;           // 定时器

   private slots:
    void updateCountdown();  // 每秒更新倒计时

};

#endif  // PAYDIALOG_H
