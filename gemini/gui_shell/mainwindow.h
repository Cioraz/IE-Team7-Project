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
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void handleInput();

private:
    Ui::MainWindow *ui;
    Shell shell;
    QString currentInput;
    QString pendingCommand;
    int promptPos;   // Tracks the position of the prompt

    void appendOutput(const QString &text); // <-- Only declare once
};

#endif // MAINWINDOW_H
