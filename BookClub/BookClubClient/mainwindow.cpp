#include "mainwindow.h"
// #include "ui_mainwindow.h" // این فایل فعلاً وجود ندارد

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
// , ui(new Ui::MainWindow) // موقتاً غیرفعال
{
    // ui->setupUi(this); // موقتاً غیرفعال

    // یک عنوان ساده برای پنجره سرور
    this->setWindowTitle("BookClub Server - DB Testing");
    this->resize(300, 200);
}

MainWindow::~MainWindow()
{
    // delete ui; // موقتاً غیرفعال
}
