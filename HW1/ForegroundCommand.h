//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_FOREGROUNDCOMMAND_H
#define OSWET1_FOREGROUNDCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"
#include "SmallShell.h"


class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    explicit ForegroundCommand(JobsList &jobsList);

    ~ForegroundCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;

    static bool areParamsValid(const vector<string> &args);

    shared_ptr<JobsList::JobEntry> getJob(const vector<string> &args);

    static int continueIfJobIsStopped(const shared_ptr<JobsList::JobEntry> &job);

    static void waitForJobToFinish(const shared_ptr<JobsList::JobEntry> &job);
};


#endif //OSWET1_FOREGROUNDCOMMAND_H
