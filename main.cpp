#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <time.h>
using namespace std;

// methods declaration
void execute(pair <char**, int> command);
pair <char**, int> parse();
void sigint_handler(int signo);

int main() {
    // remove the old log file and initialize the signal handler.
    remove("log.txt");
    signal(SIGCHLD, sigint_handler);
    while(1){
        execute(parse());
    }
    return 0;
}

// shell commands execution method.
// command: the input command after being splitted into char**...
// as the first in the pair, and the background variable in second.
void execute(pair <char**, int> command){
    char** args = command.first;

    // if the command is exit, terminate.
    if(strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    // fork new process to execute command.
    pid_t pid = fork();
    int state;
    if(pid == 0){
        // child execute the command.
        execvp(args[0], args);
        // show error message when there's an error in the command.
        perror(args[0]);
    } else if (pid > 0){
        // parent waits if it's not a background execution.
        if(command.second == 0){
            pid_t pp = wait(&state);
        }
    } else {
        // if fork failed.
        perror("Fork failed");
        exit(1);
    }
}

// parse method that take the input from the user and split it.
// it returns a pair with the first element as the command splitted..
// in the form of char**. and the second element as the background variable,.
// indecates if the process needs to be executed in background or not.
pair <char**, int> parse(){
    string input;
    cout << ">> ";
    // take user input.
    getline(cin, input);

    // reform the command to have only one space saperator
    // without any leading or trailing spaces.
    regex spaces("\\s+");
    input = regex_replace(input, spaces, " ");
    if(input[0] == ' '){
        input.replace(0,1,"");
    }
    if(input[input.length() - 1] == ' '){
        input.replace(input.length() - 1,1,"");
    }

    // set the background variable to 1 if the command has '&' at the end.
    // remove the & and any spaces before it.
    int bg = input.back() == '&' ? 1 : 0;
    if(bg){
        do {
            input.replace(input.length() - 1,1,"");
        } while(input.back() == ' ');
    }

    // split the command.
    vector <string> tokens;
    stringstream check1(input);
    string temp;
    while(getline(check1,temp,' ')){
        tokens.push_back(temp);
    }

    // convert the command from vector of strings to char**
    char ** arr = new char*[tokens.size()+1];
    for(size_t i = 0; i < tokens.size(); i++){
        arr[i] = new char[tokens[i].size() + 1];
        strcpy(arr[i],tokens[i].c_str());
    }
    arr[tokens.size()] = NULL;
    pair <char**, int> result(arr, bg);
    return result;
}

// write to log file whenever process terminates.
void sigint_handler(int signo) {
    FILE *fp;
    fp = fopen("log.txt", "a");
    time_t my_time = time(NULL);
    fprintf(fp, "Child process was terminated at: %s \n", ctime(&my_time));
    fclose(fp);
}
