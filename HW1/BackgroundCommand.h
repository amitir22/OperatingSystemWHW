//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_BACKGROUNDCOMMAND_H
#define OSWET1_BACKGROUNDCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"


class BackgroundCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    explicit BackgroundCommand(JobsList &jobsList);

    ~BackgroundCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;

    static bool areParamsValid(const vector<string> &args);

    shared_ptr<JobsList::JobEntry> getJob(const vector<string> &args);
};


#endif //OSWET1_BACKGROUNDCOMMAND_H
