//
// Created by shvmo and amit on 13/11/2020.
//

#include <csignal>
#include <iostream>
#include "BackgroundCommand.h"


BackgroundCommand::BackgroundCommand(JobsList &jobsList) : jobsList(jobsList) {
}

BackgroundCommand::~BackgroundCommand() = default;

void BackgroundCommand::execute(vector<string> args) {
    if (!areParamsValid(args)) {
        cerr << "smash error: bg: invalid arguments" << endl;
    } else {
        shared_ptr<JobsList::JobEntry> job = getJob(args);

        if (job != nullptr) {
            if (!(job->isStopped)) {
                cerr << "smash error: bg: job-id " << job->jobId << " is already running in the background" << endl;
            } else {
               cout << job->command << " : " << job->pid << endl;

                this->jobsList.removeJobByPid(job->pid);
                this->jobsList.addJob(job->jobId, job->pid, job->command);

                if (kill(job->pid, SIGCONT) == -1) {
                    perror("smash error: kill failed");
                    // reverse in case of error
                    this->jobsList.removeJobByPid(job->pid);
                    this->jobsList.addJob(job->jobId, job->pid, job->command, true);
                }
            }
        }
    }
}

bool BackgroundCommand::areParamsValid(const vector<string> &args) {
    if (args.empty()) {
        return true;
    } else {
        char *endPointer;
        strtol(args[0].c_str(), &endPointer, 10);

        return (*endPointer) == '\0' && endPointer != args[0].c_str() && args.size() == 1;
    }
}

shared_ptr<JobsList::JobEntry> BackgroundCommand::getJob(const vector<string> &args) {
    shared_ptr<JobsList::JobEntry> job;

    if (args.empty()) {
        job = jobsList.getLastStoppedJob();

        if (job == nullptr) {
          cerr <<"smash error: bg: there is no stopped jobs to resume" << endl;
        }
    } else {
        int jobId = strtol(args[0].c_str(), nullptr, 10);

        job = jobsList.getJobById(jobId);

        if (job == nullptr) {
           cerr << "smash error: bg: job-id " << jobId << " does not exist" << endl;
        }
    }

    return job;
}