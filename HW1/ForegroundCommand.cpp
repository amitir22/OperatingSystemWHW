//
// Created by shvmo and amit on 13/11/2020.
//

#include <wait.h>
#include <cassert>
#include <iostream>
#include "ForegroundCommand.h"
#include "SmallShell.h"

const string ALL_DIGITS = "0123456789";


ForegroundCommand::ForegroundCommand(JobsList &jobsList) : jobsList(jobsList) {
}

ForegroundCommand::~ForegroundCommand() = default;

void ForegroundCommand::execute(vector<string> args) {
    if (!areParamsValid(args)) {
        cerr << "smash error: fg: invalid arguments" << endl;
    } else {
        shared_ptr<JobsList::JobEntry> job = getJob(args);
        SmallShell &shell = SmallShell::getInstance();

        if (job != nullptr) {
            cout << job->command + " : " + to_string(job->pid) << endl;

            if (continueIfJobIsStopped(job) == 1) {
                this->jobsList.removeJobByPid(job->pid);

                int prevPid = shell.getCurrentPidForegroundProcess();
                int prevJobId = shell.getCurrentJobIdForegroundProcess();

                shell.setCurrentPidForegroundProcess(job->pid);
                shell.setCurrentCommand(job->command);
                shell.setCurrentJobIdForegroundProcess(job->jobId);

                waitForJobToFinish(job);

                shell.setCurrentPidForegroundProcess(prevPid);
                shell.setCurrentJobIdForegroundProcess(prevJobId);
            }
        }
    }
}

bool ForegroundCommand::areParamsValid(const vector<string> &args) {
    if (args.empty()) {
        return true;
    } else {
        char *endPointer;
        strtol(args[0].c_str(), &endPointer, 10);

        return (*endPointer) == '\0' && endPointer != args[0].c_str() && args.size() == 1;
    }
}

shared_ptr<JobsList::JobEntry> ForegroundCommand::getJob(const vector<string> &args) {
    shared_ptr<JobsList::JobEntry> job;

    if (args.empty()) {
        job = jobsList.getLastJob();

        if (job == nullptr) {
            cerr << "smash error: fg: jobs list is empty" << endl;
        }
    } else {
        int jobId = strtol(args[0].c_str(), nullptr, 10);

        job = jobsList.getJobById(jobId);

        if (job == nullptr) {
            cerr << "smash error: fg: job-id " + to_string(jobId) + " does not exist" << endl;
        }
    }

    return job;
}

int ForegroundCommand::continueIfJobIsStopped(const shared_ptr<JobsList::JobEntry> &job) {
    if (job->isStopped) {
        int killStatus = kill(job->pid, SIGCONT);

        if (killStatus == -1) {
            perror("smash error: kill failed");

            return 0;
        }
    }

    return 1;
}

void ForegroundCommand::waitForJobToFinish(const shared_ptr<JobsList::JobEntry> &job) {
    int waitStatus = waitpid(job->pid, nullptr, WUNTRACED);

    if (waitStatus == -1) {
        perror("smash error: waitpid failed");
    }
}
