//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_REDIRECTIONCOMMAND_H
#define OSWET1_REDIRECTIONCOMMAND_H

#include "Commands.h"
#include "JobsList.h"
#include "CommandExecutor.h"


class RedirectionCommand {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(JobsList &jobsList);

    ~RedirectionCommand();

    void execute(vector<string> args, bool isBackground, bool append);

private:
    JobsList &jobsList;
    CommandExecutor commandExecutor;

    void runRedirectedCommand(vector<string> args, bool append);
};


#endif //OSWET1_REDIRECTIONCOMMAND_H
