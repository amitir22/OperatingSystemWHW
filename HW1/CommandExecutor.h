//
// Created by amit and shvmo on 11/26/20.
//

#ifndef OSWET1_COMMANDEXECUTOR_H
#define OSWET1_COMMANDEXECUTOR_H

#include <string>
#include <memory>
#include "Commands.h"
#include "JobsList.h"
#include "ExternalCommand.h"
#include <map>

using namespace std;

class CommandExecutor {
public:
    explicit CommandExecutor(JobsList &jobsList);

    ~CommandExecutor();

    void executeCommand(const string &command);

private:
    JobsList &jobsList;
    ExternalCommand externalCommand;

    map<string, unique_ptr<Command>> builtinCommandsArgToCommand;

    static string ltrim(const string &s);

    static string rtrim(const string &s);

    static string trim(const string &s);

    static vector<string> parseCommandLine(const char *cmd_line);

    void initCommandVariables();


};


#endif //OSWET1_COMMANDEXECUTOR_H
