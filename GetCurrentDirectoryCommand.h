//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_GETCURRENTDIRECTORYCOMMAND_H
#define OSWET1_GETCURRENTDIRECTORYCOMMAND_H

#include "BuiltInCommand.h"


class GetCurrentDirectoryCommand : public BuiltInCommand {
public:
    GetCurrentDirectoryCommand();

    ~GetCurrentDirectoryCommand() override;

    void execute(vector<string> args) override;
};


#endif //OSWET1_GETCURRENTDIRECTORYCOMMAND_H
