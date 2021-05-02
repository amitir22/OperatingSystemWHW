//
// Created by shvmo and amit on 13/11/2020.
//

#include <unistd.h>
#include <wait.h>
#include "PipeCommand.h"


const char BACKGROUND_KEYWORD = '&';

PipeCommand::PipeCommand(JobsList &jobsList) : jobsList(jobsList), commandExecutor(jobsList) {
}

PipeCommand::~PipeCommand() = default;

void PipeCommand::execute(vector<string> args) {
    int readWritePipes[2];
    if (pipe(readWritePipes) == -1) {
        perror("smash error: pipe failed");
    }

    int firstCommandPid = fork();
    int secondCommandPid;

    if (firstCommandPid == -1) {
        perror("smash error: fork failed");
    } else if (firstCommandPid == 0) {
        runFirstPipeCommand(args, readWritePipes);
    } else {
        secondCommandPid = fork();

        if (secondCommandPid == -1) {
            perror("smash error: fork failed");
        } else {
            if (secondCommandPid == 0) {
                runSecondPipeCommand(args, readWritePipes);
            }

            closePipeFDs(readWritePipes);

            waitForCommandToFinish(args, firstCommandPid, secondCommandPid);
        }
    }
}


void PipeCommand::runFirstPipeCommand(const vector<string> &args, const int *readWritePipes) {
    redirectFirstCommandOutputToPipe(readWritePipes);

    this->commandExecutor.executeCommand(args[0]);

    close(1);

    exit(0);
}

void PipeCommand::runSecondPipeCommand(const vector<string> &args, const int *readWritePipes) {
    redirectSecondCommandInputToPipe(readWritePipes);

    commandExecutor.executeCommand(args[1]);

    exit(0);
}

void PipeCommand::waitForCommandToFinish(const vector<string> &args, int firstCommandPid, int secondCommandPid) {
    bool isBackGroundCommand = args[args.size() - 1].find(BACKGROUND_KEYWORD) != string::npos;

    if (!isBackGroundCommand) {
        waitpid(firstCommandPid, nullptr, 0);
        waitpid(secondCommandPid, nullptr, 0);
    } else {
        jobsList.addJob(firstCommandPid, args[0]);
        jobsList.addJob(secondCommandPid, args[1]);
    }
}

void PipeCommand::redirectFirstCommandOutputToPipe(const int *readWritePipes) {
    dup2(readWritePipes[1], 1);
    closePipeFDs(readWritePipes);
}

void PipeCommand::redirectSecondCommandInputToPipe(const int *readWritePipes) {
    dup2(readWritePipes[0], 0);
    closePipeFDs(readWritePipes);
}

void PipeCommand::closePipeFDs(const int *readWritePipes) {
    close(readWritePipes[0]);
    close(readWritePipes[1]);
}

