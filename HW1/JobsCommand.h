//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_JOBSCOMMAND_H
#define OSWET1_JOBSCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"


class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    explicit JobsCommand(JobsList &jobsList);

    ~JobsCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;
};

#endif //OSWET1_JOBSCOMMAND_H
