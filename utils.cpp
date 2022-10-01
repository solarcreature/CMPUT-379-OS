#include "headers.h"

void sleep_cmd(string line) {

    size_t sz;

    try {

        long long time = stoll(line,&sz);
        sleep(time);

    }

    catch(exception &E) {
        cout << "Incorrect Time\n";
    }

}

void exit_shell(ProcessTable pt) {
    pt.clean();
    pt.display_time(1);
}

void call_jobs(ProcessTable pt) {
    pt.jobs();
}

pid_t spawn(vector<string> lines, bool *background) {
    
    char *args[MAX_ARGS + 1] = {NULL};
    char *ifp;
    bool isInput = false;
    char *ofp;
    bool isOutput = false;
    pid_t pid;
    int argIndex = 0;
    int status = 0;
    string fileName;

    for (unsigned int i = 0; i < lines.size(); i++) {
        
        char c = lines[i][0];

        if (i == lines.size() - 1 and c == '&') {
            *background = true;
        }

        else if (c != '<' and c != '>') {
            args[argIndex] = (char *) lines[i].c_str();
            argIndex++;
        }

        else if (c == '>') {
            isOutput = true;
            fileName = lines[i].substr(1,lines[i].size() - 1);
            ofp = (char *) fileName.c_str();
        }

        else {
            isInput = true;
            fileName = lines[i].substr(1,lines[i].size() - 1);
            ifp = (char *) fileName.c_str();
        }

    }

    pid = fork();

    if (pid < 0) {
        
        perror("Fork failed");
        return 0;
        
    }

    else if (pid == 0) {

        if(isOutput) {

            close(STDOUT_FILENO);
            if(open(ofp, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU) < 0) {
                perror("Output file failed to open");
            }

        }

        if(isInput) {

            close(STDIN_FILENO);
            if(open(ifp, O_RDONLY, S_IRWXU) < 0) {
                perror("Input file failed to open");
            }
        }

        execvp(args[0],args);
        _exit(0);
    }

    else {

        if( !(*background) ) {
            wait(&status);
        }

        return pid;

    }

}

