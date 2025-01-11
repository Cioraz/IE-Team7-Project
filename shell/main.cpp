#include <iostream>
#include "shell.h"
using namespace std;

int main() {
    Shell shell;
    while (true) {
        cout << "> ";
        string input = shell.readLine();
        vector<string> args = shell.parseLine(input);
        if (args.empty()) continue;
        if (shell.execute(args) == 0) break; 
    }
    return 0;
}