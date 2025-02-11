#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "shell.h"

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

protected:
    // Declare eventFilter here
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void handleInput();

private:
    Ui::MainWindow *ui;
    Shell shell;
    QString currentInput;
    void appendOutput(const QString &text);
};
#endif // MAINWINDOW_H
