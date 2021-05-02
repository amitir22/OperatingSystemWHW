//
// Created by shvmo and amit on 13/11/2020.
//

#include <csignal>
#include <iostream>
#include "KillCommand.h"

const string ALL_DIGITS = "0123456789";


KillCommand::KillCommand(JobsList &jobsList) : jobsList(jobsList) {
}

KillCommand::~KillCommand() = default;

void KillCommand::execute(vector<string> args) {
    if (!areParamsValid(args)) {
        cout << "smash error: kill: invalid arguments" << endl;
    } else {
        int jobId = atoi(args[1].c_str());
        int signalNumber = extractSignalNumberFromArgs(args);

        const shared_ptr<JobsList::JobEntry> job = this->jobsList.getJobById(jobId);

        if (job == nullptr) {
            cout << "smash error: kill: job-id " + to_string(jobId) + " does not exist" << endl;
        } else {
            cout << "signal number " + to_string(signalNumber) + " was sent to pid " + to_string(job->pid) << endl;
            kill(job->pid, signalNumber);
        }
    }
}

bool KillCommand::areParamsValid(const vector<string> &args) {
    // TODO: make sure if this 'fix' is needed
    bool areThereTwoArgs = args.size() == 2;

    if (areThereTwoArgs) {
        bool isSecondArgOnlyDigits = args[1].find_first_not_of(ALL_DIGITS) == string::npos;
        bool isFirstArgPrefixCorrect = args[0][0] == '-';
        const string &signalNumString = args[0].substr(1, args[0].size() - 1);
        int signalNumber = extractSignalNumberFromArgs(args);

        bool isFirstArgSuffixValidDigits =
                signalNumString.find_first_not_of(ALL_DIGITS) == string::npos &&
                signalNumber >= 1 && signalNumber <= 32;

        return isFirstArgPrefixCorrect && isFirstArgSuffixValidDigits && isSecondArgOnlyDigits;
    }

    return false;
}


int KillCommand::extractSignalNumberFromArgs(const vector<string> &args) {
    const string &signalNumString = args[0].substr(1, args[0].size() - 1);
    int signalNumber = atoi(signalNumString.c_str());

    return signalNumber;
}
