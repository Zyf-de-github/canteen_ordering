#define QRCODE "QRcode.jpg"  // 二维码图片
#define REMAINTIME 60        // 最大等待支付秒数

#include "paydialog.h"

#include <QMessageBox>
#include <QTimer>

#include "ui_paydialog.h"

PayDialog::PayDialog(QWidget* parent, double totalMoney) : QDialog(parent), ui(new Ui::PayDialog)
{
    ui->setupUi(this);

    // 固定窗口大小
    this->setMinimumSize(300, 300);
    this->setMaximumSize(300, 300);
    this->resize(this->minimumSize());

    // 二维码 QLabel 设置
    ui->label->setMinimumSize(150, 150);
    ui->label->setMaximumSize(150, 150);
    ui->label->setPixmap(QPixmap(QRCODE));
    ui->label->setScaledContents(true);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 显示金额和倒计时
    remainingSeconds = REMAINTIME;  // 倒计时 REMAINTIME 秒
    ui->lineEdit_2->setText(QString("请支付  ￥%1  元").arg(totalMoney));
    PayDialog::updateCountdown();

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PayDialog::updateCountdown);
    timer->start(1000);  // 每秒触发一次
}

PayDialog::~PayDialog()
{
    delete ui;
}

// 每秒更新倒计时
void PayDialog::updateCountdown()
{
    remainingSeconds--;
    if (remainingSeconds >= 0)
    {
        ui->lineEdit->setText(QString("剩余 %1 秒").arg(remainingSeconds));
    }
    else
    {
        timer->stop();  // 停止定时器
        QMessageBox::warning(this, "支付超时", "未在规定时间内付款，订单已取消！");
        this->reject();  // 自动关闭对话框，相当于取消
    }
}
