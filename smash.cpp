#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

//done
const std::string WHITESPACE = " \n\r\t\f\v";
int main(int argc, char* argv[]) {

    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    struct sigaction my_alarm_struct;
    my_alarm_struct.sa_flags=SA_RESTART;
    if(sigaction(SIGALRM , &my_alarm_struct,NULL) == -1){
        perror("smash error: failed to set alarm handler");
    }
    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.smash_display_line;
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if (std::string(cmd_line).find_first_not_of(WHITESPACE) == std::string::npos) {
            continue;
        }
        smash.executeCommand(cmd_line.c_str());
    }
    return 0;

}