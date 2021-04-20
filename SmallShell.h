//
// Created by shvmo and amit on 13/11/2020.
//

#ifndef OSWET1_SMALLSHELL_H
#define OSWET1_SMALLSHELL_H

#include "Commands.h"
#include <map>
#include <memory>
#include "JobsList.h"
#include <csignal>
#include "ExternalCommand.h"
#include "PipeCommand.h"
#include "CommandExecutor.h"
#include "RedirectionCommand.h"

using namespace std;


class SmallShell {
public:
    SmallShell(SmallShell const &) = delete; // disable copy ctor
    void operator=(SmallShell const &) = delete; // disable = operator
    static SmallShell &getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell();

    bool executeCommand(const char *cmd_line);

    string getCommandPromptName();

    string setCommandPromptName(string name);

    string getCurrentDirectory();

    void setCurrentDirectory(string newDirectory);

    string getPreviousDirectory();

    void setPreviousDirectory(string prevDirectory);

    JobsList &getJobsList();

    int getCurrentPidForegroundProcess() const;

    void setCurrentPidForegroundProcess(int pid);

    int getCurrentJobIdForegroundProcess() const;

    void setCurrentJobIdForegroundProcess(int jobId);

    string getCurrentCommand();

    void setCurrentCommand(string command);

private:
    PipeCommand pipeCommand;
    RedirectionCommand redirectionCommand;
    CommandExecutor commandExecutor;
    JobsList jobsList;
    string commandPromptName;
    string currentDirectory;
    string previousDirectory;
    int currentPidForegroundProcess{};
    int currentJobIdForegroundProcess{};
    string currentCommand;

    static string ltrim(const string &s);

    static string rtrim(const string &s);

    static string trim(const string &s);

    static vector<string> parseCommandLine(const char *cmd_line);

    static vector<string> splitCommand(const char *cmd_line, char delimiter);

    SmallShell();

    void initEnvironment();

    static void initSignalHandlers();

    static void handleIntSignal(int sig, siginfo_t *siginfo, void *context);

    static void handleStpSignal(int sig, siginfo_t *siginfo, void *context);

    static bool isPipeCommand(const char *cmd_line);

    static bool isRedirectionCommand(const char *cmd_line);

    static vector<string> tokenizeRedirectionCommand(const string &cmdLine);

    static bool isBackgroundCommand(const char *cmd_line);

    static string removeTrailingBackgroundChar(const string &bashCommand);
};


#endif //OSWET1_SMALLSHELL_H
