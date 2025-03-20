#ifndef SHELL_H
#define SHELL_H

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <regex>
#include <vector>
#include <string>
#include <map>
#include <cstdio>
#include <algorithm>

#define LSH_TOKEN_DELIM " \t\r\n\a"

class Shell {
public:
    Shell();
    void shell_loop();
    std::string readLine();
    std::vector<std::string> parseLine(std::string& input);
    int execute(std::vector<std::string>& args);
    int launch(std::vector<std::string>& args);
    int cd(std::string& path);
    int help();
    int exit();
    int pwd();
    int echo(std::string& message);
    int kills(pid_t processId);
    void Pipelines(std::vector<std::string>& command1, std::vector<std::string>& command2);
    std::string substituteVariable(const std::string& args);
    std::string processNLPCommand(const std::string& nlInput);
    std::string getGeminiFallback(const std::string& query);

private:
    std::vector<std::string> builtin_str = {"cd", "help", "exit", "pwd", "echo", "kills"};
    std::map<std::string, std::string> environmentVariables;
};

#endif
