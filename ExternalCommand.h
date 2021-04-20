//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_EXTERNALCOMMAND_H
#define OSWET1_EXTERNALCOMMAND_H

#include "Commands.h"
#include "JobsList.h"

class ExternalCommand : public Command {
public:
    explicit ExternalCommand(JobsList &jobsList);

    ~ExternalCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;

    static void runForegroundExternalCommand(const vector<string> &args);

    static void execvBashCommandInChildProcess(const vector<string> &args);

    void runBackgroundExternalCommand(const vector<string> &args);

    static void runBackgroundChildProcess(const vector<string> &args, const int *readWritePipes);

    static string getBashCommand(const vector<string> &args);

    static string removeTrailingBackgroundChar(const string &bashCommand);
};


#endif //OSWET1_EXTERNALCOMMAND_H
