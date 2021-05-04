//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_COMMANDSHISTORY_H
#define OSWET1_COMMANDSHISTORY_H


#include <vector>

class CommandsHistory {

protected:
    class CommandHistoryEntry {
        // TODO: Add your data members
    };

public:
    CommandsHistory();

    ~CommandsHistory() {}

    void addRecord(const char *cmd_line);

    void printHistory();
};


#endif //OSWET1_COMMANDSHISTORY_H
