#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shell.h"
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit->setText("> ");
    ui->lineEdit->setCursorPosition(2);

    connect(ui->lineEdit, &QLineEdit::cursorPositionChanged, this, [this](int, int newPos) {
        if (newPos < 2) ui->lineEdit->setCursorPosition(2);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_returnPressed()
{
    Shell shell;

   
    QString userInput = ui->lineEdit->text().trimmed();

  
    if (!userInput.startsWith("> ")) {
        ui->lineEdit->setText("> ");
        ui->lineEdit->setCursorPosition(2);
        return;
    }

    std::string line = userInput.mid(2).toStdString(); // Remove "> "

    if (line.empty()) return; // Ignore empty input

    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    fflush(stdout);
    fflush(stderr);

    int savedStdout = dup(STDOUT_FILENO);
    int savedStderr = dup(STDERR_FILENO);
    if (savedStdout == -1 || savedStderr == -1) {
        perror("dup");
        return;
    }

    if (dup2(pipefd[1], STDOUT_FILENO) == -1 || dup2(pipefd[1], STDERR_FILENO) == -1) {
        perror("dup2");
        return;
    }

    ::close(pipefd[1]);  // Close unused write end

  
    vector<string> args = shell.parseLine(line);
    int status = shell.execute(args);
    (void)status; // Ignore status for now

    fflush(stdout);
    fflush(stderr);

   
    dup2(savedStdout, STDOUT_FILENO);
    dup2(savedStderr, STDERR_FILENO);
    ::close(savedStdout);
    ::close(savedStderr);

   
    string capturedOutput;
    char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        capturedOutput.append(buffer);
    }
    ::close(pipefd[0]);

   
    ui->output->append(QString::fromStdString(capturedOutput));

    
    ui->lineEdit->clear();
    ui->lineEdit->setText("> ");
    ui->lineEdit->setCursorPosition(2);
}
