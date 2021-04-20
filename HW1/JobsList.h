//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_JOBSLIST_H
#define OSWET1_JOBSLIST_H

#include "Commands.h"
#include <ctime>
#include <memory>


class JobsList {
public:
    struct JobEntry {
        time_t startTime;
        int pid;
        int jobId;
        string command;
        bool isStopped;
    };

public:
    JobsList();

    ~JobsList();

    void addJob(int pid, string command, bool isStopped = false);

    void addJob(int jobId, int pid, string command, bool isStopped = false);

    void printJobsList();

    void killAllJobs();

    void removeFinishedJobs();

    shared_ptr<JobEntry> getJobById(int jobId);

    shared_ptr<JobEntry> getJobByPid(int pid);

    void removeJobById(int jobId);

    void removeJobByPid(int pid);

    shared_ptr<JobEntry> getLastJob();

    shared_ptr<JobEntry> getLastStoppedJob();

    static int getJobElapsedTime(const JobEntry &job);

private:
    vector<JobEntry> jobs;
    int maxJobId;

    void eraseJob(int i, const JobEntry &job);
};


#endif //OSWET1_JOBSLIST_H
