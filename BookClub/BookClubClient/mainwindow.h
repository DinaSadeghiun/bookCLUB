#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

/* موقتاً کامنت شده تا ارور vtable رفع شود
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    // Ui::MainWindow *ui; // موقتاً غیرفعال
};
#endif // MAINWINDOW_H
