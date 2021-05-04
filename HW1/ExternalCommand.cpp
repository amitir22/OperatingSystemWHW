//
// Created by shvmo and amit on 13/11/2020.
//

#include "ExternalCommand.h"
#include "SmallShell.h"
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>


const char BACKGROUND_KEYWORD = '&';
const char FINISHED_ADDING_PID_TO_JOB_LIST_MSG[] = "Finished";
const int FINISHED_ADDING_PID_TO_JOB_LIST_MSG_LENGTH = 9;
const int RESET_PID_VALUE = 0;

ExternalCommand::ExternalCommand(JobsList &jobsList) : jobsList(jobsList) {
}

ExternalCommand::~ExternalCommand() = default;

void ExternalCommand::execute(vector<string> args) {
    bool isBackGroundCommand = args[args.size() - 1].find(BACKGROUND_KEYWORD) != string::npos;

    if (!isBackGroundCommand) {
        runForegroundExternalCommand(args);
    } else {
        runBackgroundExternalCommand(args);
    }
}

void ExternalCommand::runForegroundExternalCommand(const vector<string> &args) {
    int pid = fork();

    if (pid == -1) {
        perror("smash error: fork failed");
    } else {
        if (pid == 0) {
            if (setpgrp() == -1) {
                perror("smash error: setpgrp failed");
                // we want to exit if something failed to avoid this child process continuing on
                exit(1);
            }

            execvBashCommandInChildProcess(args);
        } else {
            SmallShell::getInstance().setCurrentPidForegroundProcess(pid);

            int status;
            if (waitpid(pid, &status, WUNTRACED) == -1) {
                perror("smash error: waitpid failed");
            }


            SmallShell::getInstance().setCurrentPidForegroundProcess(RESET_PID_VALUE);
        }
    }
}

void ExternalCommand::execvBashCommandInChildProcess(const vector<string> &args) {
    string bashCommand = removeTrailingBackgroundChar(getBashCommand(args));

    char *bashPath = strdup("/bin/bash");
    char *bashArg = strdup("-c");

    if (bashArg == nullptr || bashPath == nullptr) {
        // No point in freeing these resources as it's a temporary child process that will terminate right away
        // We can't really free the resources after execv anyway
        perror("smash error: strdup failed");
        exit(1);
    }

    char *execvArgs[]{
            bashPath,
            bashArg,
            strdup(bashCommand.c_str()),
            nullptr
    };

    if (execv("/bin/bash", execvArgs) == -1) {
        perror("smash error: execv failed");
        // we want to exit if something failed to avoid this child process continuing on
        exit(1);
    }
}

void ExternalCommand::runBackgroundExternalCommand(const vector<string> &args) {
    int readWritePipes[2];

    if (pipe(readWritePipes) == -1) {
        perror("smash error: pipe failed");
    }

    int pid = fork();

    if (pid == -1) {
        perror("smash error: fork failed");
    } else {
        if (pid == 0) {
            runBackgroundChildProcess(args, readWritePipes);
        } else {
            if (close(readWritePipes[0]) == -1) {
                perror("smash error: close failed");
            }

            jobsList.addJob(pid, SmallShell::getInstance().getCurrentCommand());

            // Now sending child message to continue running because we saved the new PID and can deal with it later
            if (write(readWritePipes[1],
                      FINISHED_ADDING_PID_TO_JOB_LIST_MSG,
                      FINISHED_ADDING_PID_TO_JOB_LIST_MSG_LENGTH) == -1) {
                perror("smash error: write failed");
            }
        }
    }
}

void ExternalCommand::runBackgroundChildProcess(const vector<string> &args, const int *readWritePipes) {
    // to avoid ctr-c or ctr-z on smash process effecting a background process
    if (setpgrp() == -1) {
        perror("smash error: setpgrp failed");
        // we want to exit if something failed to avoid this child process continuing on
        exit(1);
    }

    if (close(readWritePipes[1]) == -1) {
        perror("smash error: close failed");
        // we want to exit if something failed to avoid this child process to continue on
        exit(1);
    }

    char finishedMsgBuffer[FINISHED_ADDING_PID_TO_JOB_LIST_MSG_LENGTH];
    if (read(readWritePipes[0], finishedMsgBuffer, FINISHED_ADDING_PID_TO_JOB_LIST_MSG_LENGTH) == -1) {
        perror("smash error: read failed");
        // we want to exit if something failed to avoid this child process continuing on
        exit(1);
    }

    execvBashCommandInChildProcess(args);
}

string ExternalCommand::getBashCommand(const vector<string> &args) {
    string bashCommand;

    for (const string &arg : args) {
        bashCommand += arg + " ";
    }

    return bashCommand.substr(0, bashCommand.size() - 1);
}

string ExternalCommand::removeTrailingBackgroundChar(const string &bashCommand) {
    if (bashCommand[bashCommand.size() - 1] == BACKGROUND_KEYWORD) {
        // remove "& " at the end
        return bashCommand.substr(0, bashCommand.size() - 1);
    } else {
        return bashCommand;
    }
}
