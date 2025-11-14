#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJniObject>
#include <QJniEnvironment>
#include <QtCore/qnativeinterface.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void appendFromKotlin(const QString &text);

private:
    Ui::MainWindow *ui;

signals:
    void notificationActionPressed(void);

private slots:
    void onBtnExitClicked(void);
    void onBtnRandomNumberClicked(void);
    void onBtnSomeFuncClicked(void);
    void onBtnStringTestClicked(void);
    void onBtnNotificationClicked(void);
    void onNotificationActionPressed(void);
};
#endif // MAINWINDOW_H
