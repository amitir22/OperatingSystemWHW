//
// Created by amit and shvmo on 11/15/20.
//

#ifndef OSWET1_CHPROMPTCOMMAND_H
#define OSWET1_CHPROMPTCOMMAND_H

#include "BuiltInCommand.h"
#include "SmallShell.h"

class ChpromptCommand : public BuiltInCommand {
public:
    ChpromptCommand();

    ~ChpromptCommand() override;

    void execute(vector<string> args) override;
};


#endif //OSWET1_CHPROMPTCOMMAND_H
