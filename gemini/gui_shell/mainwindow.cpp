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
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , promptPos(0)
{
    ui->setupUi(this);

    // Set up the terminal-like QTextEdit
    ui->terminal->setReadOnly(false);
    ui->terminal->setFontFamily("Monospace");
    ui->terminal->setFontPointSize(10);

    // Display initial prompt and record its position.
    ui->terminal->append("> ");
    promptPos = ui->terminal->document()->toPlainText().length();

    // Install event filter for handling key presses
    ui->terminal->installEventFilter(this);

    pendingCommand = "";
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

        // Prevent deletion of the prompt area.
        if (keyEvent->key() == Qt::Key_Backspace || keyEvent->key() == Qt::Key_Delete) {
            QTextCursor cursor = ui->terminal->textCursor();
            if (cursor.position() <= promptPos) {
                return true; // Consume event to prevent deletion.
            }
        }

        // Process Enter/Return keys as before.
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            handleInput();
            return true; // Consume the event.
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::handleInput()
{
    QTextCursor cursor = ui->terminal->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString line = cursor.selectedText().mid(2); // Remove "> "

    // Local variable for command to execute.
    std::string execCommand;

    // Check if a pending command exists from a previous natural language generation.
    if (!pendingCommand.isEmpty()) {
        // If the user simply presses Enter (no new text), execute the pending command.
        if (line.trimmed().isEmpty()) {
            execCommand = pendingCommand.toStdString();
            pendingCommand.clear();
            // Fall through to execute execCommand.
        } else {
            // User typed something new; discard the pending command.
            pendingCommand.clear();
        }
    }

    // If no execCommand has been set from pendingCommand, then process the current line.
    if (execCommand.empty()) {
        if (line.isEmpty()) {
            ui->terminal->append("> ");
            promptPos = ui->terminal->document()->toPlainText().length();
            return;
        }

        // Check if the line is a natural language command.
        if (line.startsWith("nl:")) {
            std::string nlQuery = line.toStdString().substr(3); // Remove "nl:" prefix

            // Show waiting message and force UI update.
            ui->terminal->append("<span style='color: yellow;'>Generating command, please wait...</span>");
            QCoreApplication::processEvents();

            // Call the Python script (converter.py) to generate the bash command.
            QProcess* pythonProcess = new QProcess(this);
            pythonProcess->setProcessChannelMode(QProcess::MergedChannels);

            // Connect signal to handle when process finishes
            connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [this, pythonProcess](int exitCode, QProcess::ExitStatus exitStatus) {
                    QByteArray pyOutput = pythonProcess->readAllStandardOutput();
                    QString convertedCommandStr = QString::fromUtf8(pyOutput).trimmed();
                    
                    // Display the generated command
                    ui->terminal->append("<span style='color: green;'>Generated command:</span> " + convertedCommandStr);
                    ui->terminal->append("Press Enter to execute this command, or type a new command.");
                    
                    // Store the generated command for later execution
                    pendingCommand = convertedCommandStr;
                    ui->terminal->append("> ");
                    promptPos = ui->terminal->document()->toPlainText().length();
                    
                    // Clean up the process
                    pythonProcess->deleteLater();
                }
            );

            // Connect error signal
            connect(pythonProcess, &QProcess::errorOccurred, 
                [this, pythonProcess](QProcess::ProcessError error) {
                    ui->terminal->append("Error: Python process failed with error code: " + QString::number(error));
                    ui->terminal->append("> ");
                    promptPos = ui->terminal->document()->toPlainText().length();
                    
                    // Clean up the process
                    pythonProcess->deleteLater();
                }
            );

            // Start the process
            pythonProcess->start("python3", QStringList() << "converter.py" << QString::fromStdString(nlQuery));
            
            // Don't wait here - continue and let the callback handle the response
            return;  // Return immediately after starting the process
        } else {
            // Otherwise, treat the input as a normal command.
            execCommand = line.toStdString();
        }
    }

    // At this point, execCommand contains the command to be executed.
    std::vector<std::string> args = shell.parseLine(execCommand);

    // Redirect stdout and stderr to capture command output.
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
    ::close(pipefd[1]);

    int status = shell.execute(args);
    (void)status; // Status handling can be improved as needed

    fflush(stdout);
    fflush(stderr);
    dup2(savedStdout, STDOUT_FILENO);
    dup2(savedStderr, STDERR_FILENO);
    ::close(savedStdout);
    ::close(savedStderr);

    char buffer[1024];
    QString output;
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        output += QString::fromUtf8(buffer);
    }
    ::close(pipefd[0]);

    if (!output.isEmpty()) {
        ui->terminal->append(output);
    }
    ui->terminal->append("> ");
    promptPos = ui->terminal->document()->toPlainText().length();

    cursor.movePosition(QTextCursor::End);
    ui->terminal->setTextCursor(cursor);
}



         

 



void MainWindow::appendOutput(const QString &text)
{
    ui->terminal->moveCursor(QTextCursor::End);
    ui->terminal->insertPlainText(text);
}
