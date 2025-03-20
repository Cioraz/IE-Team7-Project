#include "shell.h"
#include <QApplication>
#include <QMetaObject>
#include <QCoreApplication>

std::string join(const std::vector<std::string>& tokens, char delim) {
    std::string result;
    for (const auto& token : tokens) {
        if (!result.empty()) result += delim;
        result += token;
    }
    return result;
}

Shell::Shell() {}

std::string Shell::processNLPCommand(const std::string& nlInput) {
    std::string python_cmd = "python3 app.py \"" + nlInput + "\"";
    
    FILE* pipe = popen(python_cmd.c_str(), "r");
    if (!pipe) return "echo 'NLP processing failed'";
    
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    
    // Remove trailing newline if present
    if (!result.empty() && result[result.length()-1] == '\n') {
        result.erase(result.length()-1);
    }
    return result;
}

std::string Shell::getGeminiFallback(const std::string& query) {
    std::string python_cmd = "python3 app.py \"" + query + "\"";
    
    FILE* pipe = popen(python_cmd.c_str(), "r");
    if (!pipe) return "echo 'Gemini fallback failed'";
    
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    
    // Remove trailing newline if present
    if (!result.empty() && result[result.length()-1] == '\n') {
        result.erase(result.length()-1);
    }
    return result;
}

std::string Shell::readLine() {
    std::string line;
    std::getline(std::cin, line);
    
    if (line.rfind("nl:", 0) == 0) {  // If input starts with "nl:"
        std::string original_query = line.substr(3); // Remove "nl:" prefix
        std::string primary_output = processNLPCommand(original_query);
        
        // Simple confidence check (replace with actual logic)
        if (primary_output.find("UNKNOWN") != std::string::npos) {
            return getGeminiFallback(original_query);
        }
        return primary_output;
    }
    
    return line;
}

std::vector<std::string> Shell::parseLine(std::string& inputCommand) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream buffer(inputCommand);
    
    while (std::getline(buffer, token, *LSH_TOKEN_DELIM)) { //tokenisation of command
        tokens.push_back(token);
    }
    return tokens;
}

int Shell::execute(std::vector<std::string>& args) {
    if (args[0] == "nl:auto") {  // Bypass confirmation
        args.erase(args.begin());
        return launch(args);
    }
    
    std::cout << "Generated command: " << join(args, ' ') << std::endl;
    std::cout << "Execute? [Y/n]: ";
    std::string confirm;
    std::getline(std::cin, confirm);
    
    if (confirm.empty() || std::tolower(confirm[0]) == 'y') {
        return launch(args);
    }
    return 1;
}

int Shell::launch(std::vector<std::string>& arguments) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        std::vector<char*> c_args;
        for (auto& arg : arguments) {
            c_args.push_back(const_cast<char*>(arg.c_str()));
        }
        c_args.push_back(nullptr);
        if (execvp(c_args[0], c_args.data()) == -1) {
            std::perror("lsh");
        }
        std::exit(0);
    } else if (pid < 0) {
        std::perror("lsh");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;  
}

int Shell::cd(std::string& path) {
    if (path.empty()) {
        std::cerr << "lsh: expected argument to \"cd\"\n";
    } else {
        if (chdir(path.c_str()) != 0) {
            std::perror("lsh");
        }
    }
    return 1;
}

int Shell::help() {
    std::cout << "Built-in functions present in this shell:\n";
    for (int i = 0; i < builtin_str.size(); i++) {
        std::cout << "  " << builtin_str[i] << std::endl;
    }
    return 1;
}

int Shell::exit() {
    std::cerr << "Exit command received. Closing application...\n";
    QCoreApplication::exit(0);
    return 0;
}

int Shell::pwd() {
    char* cwd = getcwd(NULL, 0);  
    if (cwd) {
        std::cout << cwd;
        return 1;
    } else {
        std::perror("pwd");  
        return -1;
    }
}

int Shell::echo(std::string& message) {
    std::cout << message << std::endl;
    return 1;  
}

int Shell::kills(pid_t pid) {
    int sig = SIGTERM;  
    if (kill(pid, sig) == 0) {     
        return 0;  
    } else {
        std::perror("Error sending signal");
        return -1; 
    }
}

void Shell::Pipelines(std::vector<std::string>& command1, std::vector<std::string>& command2) {
    int fd[2];
    pipe(fd);

    if (fork() != 0) { // parent process  
        close(fd[0]);     // Close the read end of the pipe      
        dup2(fd[1], STDOUT_FILENO); // Redirect standard output to the write end of the pipe
        close(fd[1]); // Close the write end of the pipe
        if (!execute(command1))
            std::cerr << "Error\n";
    } else {
        close(fd[1]); // Close the write end of the pipe
        dup2(fd[0], STDIN_FILENO); // Redirect standard input to the read end of the pipe
        close(fd[0]); // Close the read end of the pipe

        if (!execute(command2))
            std::cerr << "Error\n";
    }
}

std::string Shell::substituteVariable(const std::string& args) {
    std::string result = args;
    std::regex var_regex(R"(\$\{?([A-Za-z_][A-Za-z0-9_]*)\}?)");
    
    // Implement variable substitution logic here
    return result;
}

void Shell::shell_loop() {
    std::string line;
    std::vector<std::string> args;
    int status;

    do {
        std::cout << "> ";
        line = readLine();
        args = parseLine(line);
        status = execute(args);
    } while (status);
}
