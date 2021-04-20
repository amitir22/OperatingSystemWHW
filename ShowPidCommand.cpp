//
// Created by shvmo and amit on 13/11/2020.
//

#include "ShowPidCommand.h"
#include <iostream>
#include <unistd.h>

const int PID = getpid();


ShowPidCommand::ShowPidCommand() = default;

ShowPidCommand::~ShowPidCommand() = default;

void ShowPidCommand::execute(vector<string> args) {
    std::cout << "smash pid is " + to_string(PID) << std::endl;
}
