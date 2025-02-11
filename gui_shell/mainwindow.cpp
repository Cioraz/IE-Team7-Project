#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up the terminal-like QTextEdit
    ui->terminal->setReadOnly(false);
    ui->terminal->setFontFamily("Monospace");
    ui->terminal->setFontPointSize(10);

    // Display initial prompt
    ui->terminal->append("> ");

    // Install event filter for handling key presses
    ui->terminal->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Implement eventFilter for handling key events
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->terminal && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            handleInput();
            return true; // Consume the event
        }
    }
    return QMainWindow::eventFilter(obj, event); // Pass unhandled events to base class
}

void MainWindow::handleInput()
{
    QTextCursor cursor = ui->terminal->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText().mid(2); // Remove "> "

    if (line.isEmpty()) {
        ui->terminal->append("> ");
        return;
    }

    // Process the command
    std::string command = line.toStdString();
    std::vector<std::string> args = shell.parseLine(command);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    int savedStdout = dup(STDOUT_FILENO);
    int savedStderr = dup(STDERR_FILENO);

    if (dup2(pipefd[1], STDOUT_FILENO) == -1 || dup2(pipefd[1], STDERR_FILENO) == -1) {
        perror("dup2");
        return;
    }

    ::close(pipefd[1]); // Explicitly use POSIX close()

    int status = shell.execute(args);
    (void)status; // Ignore status for now

    fflush(stdout);
    fflush(stderr);

    dup2(savedStdout, STDOUT_FILENO);
    dup2(savedStderr, STDERR_FILENO);
    ::close(savedStdout); // Explicitly use POSIX close()
    ::close(savedStderr); // Explicitly use POSIX close()

    char buffer[1024];
    QString output;
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output += QString::fromUtf8(buffer);
    }
    ::close(pipefd[0]); // Explicitly use POSIX close()

    // Append the output to the terminal
    if (!output.isEmpty()) {
        ui->terminal->append(output);
    }

    // Add new prompt
    ui->terminal->append("> ");

    // Move cursor to the end
    cursor.movePosition(QTextCursor::End);
    ui->terminal->setTextCursor(cursor);
}

void MainWindow::appendOutput(const QString &text)
{
    ui->terminal->moveCursor(QTextCursor::End);
    ui->terminal->insertPlainText(text);
}
