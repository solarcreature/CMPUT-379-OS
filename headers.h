#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

#define MAX_ARGS 7
#define LINE_LENGTH 100