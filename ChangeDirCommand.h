//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_CHANGEDIRCOMMAND_H
#define OSWET1_CHANGEDIRCOMMAND_H

#include "BuiltInCommand.h"


class ChangeDirCommand : public BuiltInCommand {
public:
    ChangeDirCommand();

    ~ChangeDirCommand() override;

    void execute(vector<string> args) override;
};


#endif //OSWET1_CHANGEDIRCOMMAND_H
