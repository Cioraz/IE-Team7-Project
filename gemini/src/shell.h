#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <map>
using namespace std;
class Shell {
public:
    Shell();
    void shell_loop();
    string readLine();
    vector<string> parseLine(string& input);
    int execute(vector<string>& args);
    int launch (vector<string>& args);
    int cd(string& path);
    int help();
    int exit();
    int pwd();
    int echo(string& message);
    int kills(pid_t processId);
    void Pipelines( vector<string>& command1 ,vector <string> &command2);
    string substituteVariable(const string& args);
    
private:
    string inputCommand;
    vector<string> parsedCommands;
    vector<string> arguments;
    vector<string> builtin_str = {"cd", "help", "exit", "pwd", "echo", "kills"};
    map<string, string> environmentVariables;
};

#endif 