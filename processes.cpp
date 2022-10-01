#include "headers.h"

pid_t wait_handler(string line, int flag) {

    int status = 0;
    size_t sz;

    try {

        long long pid = stoll(line,&sz);
        pid_t pid_temp = waitpid(pid,&status,flag);

        if (pid_temp < -1) {
            perror("Waiting Error");
        }

        return pid_temp;
    }

    catch(exception &E) {
        cout << "Incorrect PID\n";
    }

    return -1;
}

void kill_cmd(string line, int flag) {

    size_t sz;
    
    try {

        long long pid = stoll(line,&sz);

        if(kill(pid,flag) < 0) {
            perror("Kill failed");
        }

    }

    catch(exception &E) {
        cout << "Incorrect PID\n";
    }
}

int get_seconds(string s) {

    int index = 0;
    int result = 0;
    int value;
    vector<int> conversions {1,10,60,3600};

    for (int i = s.size() - 1; i > 0; i--) {
        if(s[i] != ':') {
            value = s[i] - '0';
            result += conversions[index] * value;
            index++;
        }
    }

    return result;
}

vector<string> seperate(string s, char d) {
    
    vector<string> list;
    string buf = "";

    for (char c : s) {
        if (c == d) {
            list.push_back(buf);
            buf = "";
        }
        else
            buf += c;
    }
    list.push_back(buf);

    return list;
}

struct ActiveProcesses {
    vector<pid_t> pids;
    vector<string> commands; 
};

class ProcessTable {

    private:
        vector<pid_t> pids;
        vector<string> commands;

        string pipe(pid_t pid) {

            char buf[LINE_LENGTH];
            FILE *fp;
            string result;

            fp = popen(("ps --no-header -s --pid " + to_string(pid)).c_str(),"r");

            if (!fp) {
                perror("popen() failed");
                _exit(0);
            }
            
            while (fgets(buf,LINE_LENGTH,fp) != NULL)
                result += buf;
            pclose(fp);
            
            return result;
        }

        vector<time_t> get_total_time() {

            vector<time_t> times;
            struct rusage usage;
            time_t child_time;

            getrusage(RUSAGE_CHILDREN,&usage);
            
            child_time = usage.ru_stime.tv_sec;
            times.push_back(child_time);

            child_time = usage.ru_utime.tv_sec;
            times.push_back(child_time);

            return times;
        }

        struct ActiveProcesses get_active() {

            vector<pid_t> active_pids;
            vector<string> active_cmds;
            pid_t pid;
            struct ActiveProcesses active_values;
            
            for (unsigned int i = 0; i < pids.size(); i++) {

                pid = wait_handler(to_string(pids[i]),1);   

                if (pid == 0) {
                    active_pids.push_back(pids[i]);
                    active_cmds.push_back(commands[i]);
                }
            }

            active_values.pids = active_pids;
            active_values.commands = active_cmds;

            return active_values;
        }

        void print_table() {

            int activeCount = 0;
            struct ActiveProcesses active_values = get_active();
            
            cout << "\nRunning processes:\n";

            if (active_values.pids.size() == 0) 
                cout << "Processes =\t 0 active\n";

            else {
                cout << "#  PID S SEC COMMAND\n";
                
                for (unsigned int i = 0; i < active_values.pids.size(); i++) {
                    
                    string line = pipe(active_values.pids[i]);
                    vector<string> lines = seperate(line,' ');
                    int seconds = get_seconds(lines[15]);
                    char state = lines[6][0];

                    cout << i << ": " << active_values.pids[i] << state << "   " << seconds << '\t' << active_values.commands[i] << '\n';
                    
                    if (state == 'R') {
                        activeCount ++;
                    }
                }

                cout << "Processes =\t "<< activeCount << " active\n";
            }
        }

    public:
        
        void clean() {

            struct ActiveProcesses active_values = get_active();

            if (active_values.pids.size() > 0) {
                for (unsigned int i = 0; i < active_values.pids.size(); i++) {

                    string line = pipe(active_values.pids[i]);
                    vector<string> lines = seperate(line,' ');
                    char state = lines[6][0];
                    
                    if (state == 'R') {
                        wait_handler(to_string(active_values.pids[i]),0);
                    }

                    else
                        kill_cmd(to_string(active_values.pids[i]),SIGKILL);
                }
            }
        }

        void push_pid(pid_t pid) {
            pids.push_back(pid);
        }

        void push_command(string command) {
            commands.push_back(command);
        }

        void display_time(bool ended) {

            vector<time_t> times = get_total_time();

            if (ended)
                cout << "\nResources used\n";
            else
                cout << "Completed processes:\n";

            cout << "User time =\t " << times[1] << " seconds\n";
            cout << "Sys  time =\t " << times[0] << " seconds\n";
        }

        void jobs() {
            print_table();
            display_time(0);
        }

};








