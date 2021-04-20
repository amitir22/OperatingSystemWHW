//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_HISTORYCOMMAND_H
#define OSWET1_HISTORYCOMMAND_H

#include "BuiltInCommand.h"
#include "CommandsHistory.h"


class HistoryCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    HistoryCommand(const char *cmd_line, CommandsHistory *history);

    virtual ~HistoryCommand() {}

    void execute(vector<string> args) override;
};


#endif //OSWET1_HISTORYCOMMAND_H
