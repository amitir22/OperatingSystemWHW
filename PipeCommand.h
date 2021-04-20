//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_PIPECOMMAND_H
#define OSWET1_PIPECOMMAND_H

#include "Commands.h"
#include "JobsList.h"
#include "CommandExecutor.h"


class PipeCommand : public Command {
public:
    explicit PipeCommand(JobsList &jobsList);

    ~PipeCommand() override;

    void execute(vector<string> args) override;

private:
    JobsList &jobsList;
    CommandExecutor commandExecutor;

    void runFirstPipeCommand(const vector<string> &args, const int *readWritePipes);

    void runSecondPipeCommand(const vector<string> &args, const int *readWritePipes);

    void waitForCommandToFinish(const vector<string> &args, int firstCommandPid, int secondCommandPid);

    static void redirectFirstCommandOutputToPipe(const int *readWritePipes);

    static void redirectSecondCommandInputToPipe(const int *readWritePipes);

    static void closePipeFDs(const int *readWritePipes);
};

#endif //OSWET1_PIPECOMMAND_H
