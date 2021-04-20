//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_SHOWPIDCOMMAND_H
#define OSWET1_SHOWPIDCOMMAND_H

#include "BuiltInCommand.h"


class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand();

    ~ShowPidCommand() override;

    void execute(vector<string> args) override;
};


#endif //OSWET1_SHOWPIDCOMMAND_H
