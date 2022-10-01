#include "headers.h"
#include "processes.cpp"
#include "utils.cpp"

int main() {

    ProcessTable pt;
    bool exited = false;
    bool background = false;
    string input;
    

    while (!exited) {

        background = false;

        cout << "\nSHELL379: ";
        getline(cin,input);

        vector<string> lines = seperate(input,' ');
        string cmd = lines[0];

        if (strcmp(cmd.c_str(),"exit") == 0) {
            exit_shell(pt);
            exited = true;
            return 0;
        }

        else if (strcmp(cmd.c_str(),"jobs") == 0) {
            call_jobs(pt);
        }

        else if (strcmp(cmd.c_str(),"kill") == 0) {
            kill_cmd(lines[1],SIGKILL);
        } 

        else if (strcmp(cmd.c_str(),"resume") == 0) {
            kill_cmd(lines[1],SIGCONT);
        }

        else if (strcmp(cmd.c_str(),"suspend") == 0) {
            kill_cmd(lines[1],SIGSTOP);
        }

        else if (strcmp(cmd.c_str(),"sleep") == 0) {
            sleep_cmd(lines[1]);    
        }

        else if (strcmp(cmd.c_str(),"wait") == 0) {
            wait_handler(lines[1],0);
        }

        else {

            pid_t pid = spawn(lines,&background);
            
            if (pid != 0 && background == true) {
                pt.push_pid(pid);
                pt.push_command(input);
            }
        }
    }

}

