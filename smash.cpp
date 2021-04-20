#include <iostream>
#include "Commands.h"
#include "SmallShell.h"
#include <wait.h>

int main(int argc, char *argv[]) {
    SmallShell &smash = SmallShell::getInstance();

    bool continueSmash = true;

    while (continueSmash) {
        std::cout << smash.getCommandPromptName() + "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.getJobsList().removeFinishedJobs();
        continueSmash = smash.executeCommand(cmd_line.c_str());
    }

    return 0;
}