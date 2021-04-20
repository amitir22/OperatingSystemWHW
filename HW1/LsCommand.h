//
// Created by amit and shvmo on 11/15/20.
//

#ifndef OSWET1_LSCOMMAND_H
#define OSWET1_LSCOMMAND_H

#include "BuiltInCommand.h"
#include "SmallShell.h"

class LsCommand : public BuiltInCommand {
public:
    LsCommand();

    ~LsCommand() override;

    void execute(vector<string> args) override;
};


#endif //OSWET1_LSCOMMAND_H
