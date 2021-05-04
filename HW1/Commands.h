#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include "CommandsHistory.h"
#include <vector>
#include <string>

using namespace std;

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)

class Command {
// TODO: Add your data members
public:
    explicit Command();

    virtual ~Command();

    virtual void execute(vector<string> args) = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
};

#endif //SMASH_COMMAND_H_
