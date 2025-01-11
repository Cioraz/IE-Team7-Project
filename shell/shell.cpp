#include "shell.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <regex>
#define LSH_TOKEN_DELIM " \t\r\n\a"
using namespace std;


string Shell::readLine(){
    string line;
    getline(cin,line);
    return line;
}

vector<string> Shell::parseLine(string &inputCommand) {
    vector<string> tokens;
    string token;
    istringstream buffer(inputCommand);
    
    while (getline(buffer,token,*LSH_TOKEN_DELIM)) { //tokenisation of command
        tokens.push_back(token);
    }
    return tokens;

}

int Shell::execute(vector<string>& args){
    if(args.empty() || args[0].empty()){
        return 1;
    }
    for(int i = 0;i < builtin_str.size(); ++i){
        if(args[0] == builtin_str[i]){              //args[0] corresponds to the command the user types in at the beginning
            return builtin_func[i](args);
        }
    }
    return launch(args);                //does launch(args) if the command entered is not found in the list of inbuilt functions
}

int Shell::launch(vector<string> &arguments) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        vector<char*> c_args;
        for (auto &arg : arguments) {
            //converting to appropriate argument type for execvp
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);
        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("lsh");
        }
        exit();
    } else if (pid < 0) {
        //forking didnt work
        perror("lsh");
    } else {
        //parent process maintaining tabs on child processes
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;  
}

int Shell::cd(string& path){
    if(path.empty()){
        cerr<<"lsh: expected argument to \"cd\""<<endl;
    }
    else{
        if(chdir(path.c_str()) != 0){      //chdir changes the current working directory by taking in a value of type
            perror("lsh");                 //const char* which is done by the c_str() function.
        }
    }
}

int Shell::help(){
    cout<<"Built_in functions present in this shell:"<<endl;
    for (int i = 0; i < builtin_str.size(); i++) {
        cout << "  " << builtin_str[i] << endl;
    }
    return 1;
}

int Shell::exit(){
    return 0;
}

int Shell::pwd(){
    char *cwd = getcwd(NULL, 0);  
    if (cwd) {
        printf("%s\n", cwd);  
        return 0;
    } else {
        perror("pwd");  
        return -1;
    }
}

void Shell::echo(string& message){
    istringstream input(message);
    string word;                    //basically dynamically takes in every individual string from the 
    bool isFirst = true;            //line of code the user writes
    while(input >> word){
        if(!isFirst){
            cout<<" ";          //to print out a space between every element after the first one
        }
        cout<<word;
        isFirst = false;
    }
    cout<<endl;
}


int Shell::kills(pid_t pid) {
    int sig = SIGTERM;  
    if (kill(pid, sig) == 0) {     
        return 0;  
    } else {
        perror("Error sending signal");  
        return -1; 
    }
}

void Shell::Pipelines(vector <string> &command1, vector <string> &command2) {
    int fd[2];
    pipe(fd);

    if (fork() != 0) { // parent process  
        close(fd[0]);     // Close the read end of the pipe      
        dup2(fd[1], STDOUT_FILENO); // Redirect standard output to the write end of the pipe
        close(fd[1]); // Close the write end of the pipe
       if(!execute(command1))
       cerr << "Error"<<endl;
    } else {
        close(fd[1]); // Close the write end of the pipe
        dup2(fd[0], STDIN_FILENO); // Redirect standard input to the read end of the pipe
        close(fd[0]); // Close the read end of the pipe

      if(!execute(command2))
       cerr << "Error"<<endl;
    }
}


string Shell::substituteVariable(const string& args){
    string result = args;
    regex var_regex(R"(\$\{?([A-Za-z_][A-Za-z0-9_]*)\}?)");                          //Regular exp to find either $VARIABLE or ${VARIABLE}. regex corresponds to regular expression.

    result = regex_replace(result, var_regex, [](const smatch& match) -> string {        //function to replace each match with envitonment variables value
        const string var_name = match[1].str();                                         //extracts variable name
        const char* value = getenv(var_name.c_str());                               //getenv gets the environment variables value

        return value ? string(value) : match[0].str();
    });

    return result;
}

