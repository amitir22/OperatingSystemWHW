#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <string>

using namespace std;

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)

class Command {
public:
    explicit Command();

    virtual ~Command();

    virtual void execute(vector<string> args) = 0;
    //virtual void prepare();
    //virtual void cleanup();
};

#endif //SMASH_COMMAND_H_
