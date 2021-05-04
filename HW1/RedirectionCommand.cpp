//
// Created by shvmo and amit on 13/11/2020.
//

#include <fcntl.h>
#include <unistd.h>
#include "RedirectionCommand.h"
#include "SmallShell.h"

const char BACKGROUND_KEYWORD = '&';

RedirectionCommand::RedirectionCommand(JobsList &jobsList) : jobsList(jobsList), commandExecutor(jobsList) {
}

RedirectionCommand::~RedirectionCommand() = default;

void RedirectionCommand::execute(vector<string> args, bool isBackground, bool append) {
    if (isBackground) {
        int pid = fork();

        if (pid == -1) {
            perror("smash error: pid failed");
        } else if (pid == 0) {
            runRedirectedCommand(args, append);

            exit(0);
        } else {
            this->jobsList.addJob(pid, SmallShell::getInstance().getCurrentCommand());
        }
    } else {
        runRedirectedCommand(args, append);
    }
}

void RedirectionCommand::runRedirectedCommand(vector<string> args, bool append) {
    string command = args[0];
    string redirection = args[1];
    string file = args[2];

    int fd;
    if (append) {
        fd = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
    } else {
        fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    }

    if (fd == -1) {
        perror("smash error: open failed");
    } else {
        int saveStdOutFd = dup(STDOUT_FILENO);

        // Redirecting file to stdout
        dup2(fd, STDOUT_FILENO);

        commandExecutor.executeCommand(command);

        // restore Fd
        dup2(saveStdOutFd, STDOUT_FILENO);

        close(fd);
        close(fd);
    }
}
