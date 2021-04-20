//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_QUITCOMMAND_H
#define OSWET1_QUITCOMMAND_H

#include "BuiltInCommand.h"
#include "JobsList.h"

class QuitCommand : public BuiltInCommand {
public:
    explicit QuitCommand(JobsList &jobsList);

    ~QuitCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;
};


#endif //OSWET1_QUITCOMMAND_H
