//
// Created by shvmo and amit on 13/11/2020.
//

#include "JobsCommand.h"

JobsCommand::JobsCommand(JobsList &jobsList) : jobsList(jobsList) {
}

JobsCommand::~JobsCommand() = default;

void JobsCommand::execute(vector<string> args) {
    jobsList.printJobsList();
}
