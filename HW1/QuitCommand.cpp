//
// Created by shvmo and amit on 13/11/2020.
//

#include "QuitCommand.h"

const string KILL_OPTION = "kill";

QuitCommand::QuitCommand(JobsList &jobsList) : jobsList(jobsList) {
}

QuitCommand::~QuitCommand() = default;

void QuitCommand::execute(vector<string> args) {
    if (!args.empty() && args[0] == KILL_OPTION) {
        this->jobsList.killAllJobs();
    }
}
