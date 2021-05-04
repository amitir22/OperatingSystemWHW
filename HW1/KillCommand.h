//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_KILLCOMMAND_H
#define OSWET1_KILLCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"


class KillCommand : public BuiltInCommand {
public:
    explicit KillCommand(JobsList &jobsList);

    ~KillCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;

    static bool areParamsValid(const vector<string> &args);

    static int extractSignalNumberFromArgs(const vector<string> &args);
};

#endif //OSWET1_KILLCOMMAND_H
