#include <unistd.h>
#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include "SmallShell.h"
#include "ChangeDirCommand.h"
#include "ExternalCommand.h"
#include <sys/wait.h>

using namespace std;

const char BACKGROUND_KEYWORD = '&';
const char PIPE_DELIMITER = '|';
const string WHITESPACE = " \n\r\t\f\v";
const int RESET_PID_VALUE = 0;
const int RESET_JOBID_VALUE = 0;

const string DEFAULT_PROPMTNAME = "smash";
const string QUIT_COMMAND_KEYWORD = "quit";
const string REDIRECTION_WITH_APPEND = ">>";
const string REDIRECTION_WITH_NO_APPEND = ">";


SmallShell::SmallShell() : pipeCommand(this->jobsList), redirectionCommand(this->jobsList),
                           commandExecutor(this->jobsList) {
    initEnvironment();

    initSignalHandlers();
}

SmallShell::~SmallShell() = default;

bool SmallShell::executeCommand(const char *cmd_line) {
    vector<string> args = parseCommandLine(cmd_line);
    setCurrentCommand(cmd_line);

    if (!args.empty()) {
        string commandArg = args[0];

        if (isPipeCommand(cmd_line)) {
            this->pipeCommand.execute(splitCommand(cmd_line, PIPE_DELIMITER));
        } else if (isRedirectionCommand(cmd_line)) {
            vector<string> tokenizedArgs = tokenizeRedirectionCommand(removeTrailingBackgroundChar(cmd_line));

            this->redirectionCommand.execute(tokenizedArgs,
                                             isBackgroundCommand(cmd_line),
                                             tokenizedArgs[1] == REDIRECTION_WITH_APPEND);
        } else {
            this->commandExecutor.executeCommand(cmd_line);
        }

        return commandArg != QUIT_COMMAND_KEYWORD;
    }

    return true;
}

string SmallShell::getCommandPromptName() {
    return this->commandPromptName;
}

string SmallShell::setCommandPromptName(string name) {
    return this->commandPromptName = std::move(name);
}

string SmallShell::getCurrentDirectory() {
    return this->currentDirectory;
}

void SmallShell::setCurrentDirectory(string newDirectory) {
    this->currentDirectory = std::move(newDirectory);
}

string SmallShell::getPreviousDirectory() {
    return this->previousDirectory;
}

void SmallShell::setPreviousDirectory(string prevDirectory) {
    this->previousDirectory = std::move(prevDirectory);
}

JobsList &SmallShell::getJobsList() {
    return this->jobsList;
}

int SmallShell::getCurrentPidForegroundProcess() const {
    return this->currentPidForegroundProcess;
}

void SmallShell::setCurrentPidForegroundProcess(int pid) {
    this->currentPidForegroundProcess = pid;
}

int SmallShell::getCurrentJobIdForegroundProcess() const {
    return this->currentJobIdForegroundProcess;
}

void SmallShell::setCurrentJobIdForegroundProcess(int jobId) {
    this->currentJobIdForegroundProcess = jobId;
}

string SmallShell::getCurrentCommand() {
    return this->currentCommand;
}

void SmallShell::setCurrentCommand(string command) {
    this->currentCommand = std::move(command);
}

string SmallShell::ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string SmallShell::rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string SmallShell::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

vector<string> SmallShell::parseCommandLine(const char *cmd_line) {
    vector<string> args;

    std::istringstream iss(trim(string(cmd_line)));

    for (std::string s; iss >> s;) {
        args.emplace_back(s);
    }

    return args;
}

vector<string> SmallShell::splitCommand(const char *cmd_line, char delimiter) {
    stringstream commandStream(trim(cmd_line));
    vector<string> parts;
    string part;

    while (getline(commandStream, part, delimiter)) {
        parts.emplace_back(part);
    }

    return parts;
}

void SmallShell::initEnvironment() {
    setCommandPromptName(DEFAULT_PROPMTNAME);

    char *dirNameArrayChar = get_current_dir_name();
    if (dirNameArrayChar == nullptr) {
        perror("smash error: get_current_dir_name failed");
        exit(1);
    }

    setCurrentDirectory(dirNameArrayChar);

    free(dirNameArrayChar);
}

void SmallShell::initSignalHandlers() {
    struct sigaction intSigAction{};

    intSigAction.sa_sigaction = &handleIntSignal;
    intSigAction.sa_flags = SA_RESTART | SA_SIGINFO;


    if (sigaction(SIGINT, &intSigAction, nullptr) == -1) {
        perror("smash error: sigaction failed");
        exit(1);
    }

    struct sigaction stpSigAction{};

    stpSigAction.sa_sigaction = &handleStpSignal;
    stpSigAction.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(SIGTSTP, &stpSigAction, nullptr) == -1) {
        perror("smash error: sigaction failed");
        exit(1);
    }
}

void SmallShell::handleIntSignal(int sig, siginfo_t *siginfo, void *context) {
    int pidProcessToStop = SmallShell::getInstance().getCurrentPidForegroundProcess();

    cout << "smash: got ctrl-C" << endl;

    if (pidProcessToStop != RESET_PID_VALUE) {
        // Killing child
        if (kill(pidProcessToStop, SIGKILL) == -1) {
            perror("smash error: kill failed");
        } else {
            cout << "smash: process " + to_string(pidProcessToStop) + " was killed" << endl;
        }
    }
}

void SmallShell::handleStpSignal(int sig, siginfo_t *siginfo, void *context) {
    SmallShell &shell = SmallShell::getInstance();
    int pidProcessToStop = shell.getCurrentPidForegroundProcess();

    cout << "smash: got ctrl-Z" << endl;

    if (pidProcessToStop != RESET_PID_VALUE) {

        // Stopping child
        if (kill(pidProcessToStop, SIGSTOP) == -1) {
            perror("smash error: kill failed");
        } else {
            // If so it means we're running a job that was previously in the job list and we need to retain the Id
            if (shell.getCurrentJobIdForegroundProcess() != RESET_JOBID_VALUE) {
                shell.getJobsList().addJob(shell.getCurrentJobIdForegroundProcess(),
                                           pidProcessToStop, shell.getCurrentCommand(), true);
            } else {
                shell.getJobsList().addJob(pidProcessToStop, shell.getCurrentCommand(), true);
            }

            cout << "smash: process " << pidProcessToStop << " was stopped" << endl;
        }
    }
}

bool SmallShell::isPipeCommand(const char *cmd_line) {
    const vector<string> &pipeCommands = splitCommand(cmd_line, PIPE_DELIMITER);

    return pipeCommands.size() == 2;
}

bool SmallShell::isRedirectionCommand(const char *cmd_line) {
    string cmdLine = cmd_line;

    return cmdLine.find(REDIRECTION_WITH_APPEND) != string::npos ||
           cmdLine.find(REDIRECTION_WITH_NO_APPEND) != string::npos;
}

vector<string> SmallShell::tokenizeRedirectionCommand(const string &cmdLine) {
    vector<string> commandAndFile;

    string currentRedirectionCommand = cmdLine.find(REDIRECTION_WITH_APPEND) == string::npos ?
                                       REDIRECTION_WITH_NO_APPEND : REDIRECTION_WITH_APPEND;

    int index = cmdLine.find(currentRedirectionCommand);

    string command = trim(cmdLine.substr(0, index));

    string file = trim(cmdLine.substr(index + currentRedirectionCommand.length()));

    commandAndFile.emplace_back(command);
    commandAndFile.emplace_back(currentRedirectionCommand);
    commandAndFile.emplace_back(file);

    return commandAndFile;
}

bool SmallShell::isBackgroundCommand(const char *cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == BACKGROUND_KEYWORD;
}

string SmallShell::removeTrailingBackgroundChar(const string &bashCommand) {
    if (bashCommand[bashCommand.size() - 1] == BACKGROUND_KEYWORD) {
        // remove "& " at the end
        return bashCommand.substr(0, bashCommand.size() - 1);
    } else {
        return bashCommand;
    }
}