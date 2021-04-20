//
// Created by shvmo and amit on 13/11/2020.
//

#include "JobsList.h"

#include <utility>
#include <cmath>
#include <signal.h>
#include <wait.h>
#include <iostream>


const int FIRST_JOB_ID = 0;
const char *NON_STOPPED_JOB_PRINT_ENTRY = "[%d] %s : %d %d secs\n";
const char *STOPPED_JOB_PRINT_ENTRY = "[%d] %s : %d %d secs (stopped)\n";

JobsList::JobsList() : maxJobId(FIRST_JOB_ID) {
}

JobsList::~JobsList() = default;

void JobsList::addJob(int pid, string command, bool isStopped) {
    JobEntry newJob{};

    newJob.isStopped = isStopped;
    newJob.jobId = ++(this->maxJobId);
    newJob.startTime = time(nullptr);
    newJob.pid = pid;
    newJob.command = std::move(command);

    this->jobs.emplace_back(newJob);
}

void JobsList::addJob(int jobId, int pid, string command, bool isStopped) {
    JobEntry newJob{};

    newJob.isStopped = isStopped;
    newJob.jobId = jobId;
    newJob.startTime = time(nullptr);
    newJob.pid = pid;
    newJob.command = std::move(command);

    if (this->jobs.empty() || this->jobs[0].jobId > jobId) {
        this->jobs.insert(this->jobs.begin(), newJob);
    } else {
        unsigned int i = 0;

        for (const JobEntry &job : this->jobs) {
            if ((i + 1 == this->jobs.size() || this->jobs[i + 1].jobId > jobId) && job.jobId < jobId) {
                this->jobs.insert(this->jobs.begin() + i + 1, newJob);

                break;
            }

            ++i;

            if (jobId >= maxJobId) {
                maxJobId = jobId;
            }
        }
    }
}

void JobsList::printJobsList() {
    for (const JobEntry &job: this->jobs) {
        if (job.isStopped) {
            cout << "[" << job.jobId << "] " << job.command << " : " << job.pid << " " << getJobElapsedTime(job)
                 << " secs (stopped)" << endl;
        } else {
            cout << "[" << job.jobId << "] " << job.command << " : " << job.pid << " " << getJobElapsedTime(job)
                 << " secs" << endl;
        }
    }
}

void JobsList::killAllJobs() {
    cout << "smash: sending SIGKILL signal to " << this->jobs.size() << " jobs:" << endl;

    for (const JobEntry &job : this->jobs) {
        cout << job.pid << ": " << job.command << endl;

        if (kill(job.pid, SIGKILL) == -1) {
            perror("smash error: kill failed");
        }
    }
}

void JobsList::removeFinishedJobs() {
    vector<JobEntry> jobsToRemove;

    for (const JobEntry &job : this->jobs) {
        int status = waitpid(job.pid, nullptr, WNOHANG);

        if (status > 0) {
            // Meaning it's a pid value of the job that finished
            jobsToRemove.emplace_back(job);
        } else if (status == -1) {
            perror("smash error: waitpid failed");
        }
    }

    for (const JobEntry &job : jobsToRemove) {
        removeJobById(job.jobId);
    }
}

shared_ptr<JobsList::JobEntry> JobsList::getJobById(int jobId) {
    for (const JobEntry &job : this->jobs) {
        if (job.jobId == jobId) {
            return make_shared<JobEntry>(job);
        }
    }

    return nullptr;
}

shared_ptr<JobsList::JobEntry> JobsList::getJobByPid(int pid) {
    for (const JobEntry &job : this->jobs) {
        if (job.pid == pid) {
            return make_shared<JobEntry>(job);
        }
    }

    return nullptr;
}

void JobsList::removeJobById(int jobId) {
    int i = 0;
    for (const JobEntry &job : this->jobs) {
        if (job.jobId == jobId) {
            eraseJob(i, job);

            break;
        }

        ++i;
    }
}

void JobsList::removeJobByPid(int pid) {
    int i = 0;
    for (const JobEntry &job : this->jobs) {
        if (job.pid == pid) {
            eraseJob(i, job);

            break;
        }

        ++i;
    }
}

void JobsList::eraseJob(int i, const JobsList::JobEntry &job) {
    if (job.jobId == maxJobId) {
        if (jobs.size() == 1) {
            maxJobId = 0;
        } else {
            maxJobId = jobs[jobs.size() - 2].jobId;
        }
    }

    jobs.erase(jobs.begin() + i);
}

shared_ptr<JobsList::JobEntry> JobsList::getLastJob() {
    return (this->jobs.empty()) ? nullptr : make_shared<JobEntry>(this->jobs[this->jobs.size() - 1]);
}

shared_ptr<JobsList::JobEntry> JobsList::getLastStoppedJob() {
    shared_ptr<JobEntry> lastStoppedJob = nullptr;

    for (const JobEntry &job : this->jobs) {
        if (job.isStopped) {
            lastStoppedJob = make_shared<JobEntry>(job);
        }
    }

    return lastStoppedJob;
}

int JobsList::getJobElapsedTime(const JobsList::JobEntry &job) {
    return (int) round(difftime(time(nullptr), job.startTime));
}
