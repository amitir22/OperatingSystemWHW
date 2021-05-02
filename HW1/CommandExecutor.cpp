//
// Created by amit and shvmo on 11/26/20.
//

#include <sstream>
#include "CommandExecutor.h"
#include "Commands.h"
#include "ChpromptCommand.h"
#include "LsCommand.h"
#include "ShowPidCommand.h"
#include "GetCurrentDirectoryCommand.h"
#include "JobsCommand.h"
#include "ChangeDirCommand.h"
#include "KillCommand.h"
#include "ForegroundCommand.h"
#include "BackgroundCommand.h"
#include "QuitCommand.h"
#include "CatCommand.h"


const string WHITESPACE = " \n\r\t\f\v";

const string CHROMPT_COMMAND_KEYWORD = "chprompt";
const string LS_COMMAND_KEYWORD = "ls";
const string SHOW_PID_COMMAND_KEYWORD = "showpid";
const string GET_CURRENT_DIRECTORY_COMMAND_KEYWORD = "pwd";
const string CD_COMMAND_KEYWORD = "cd";
const string JOBS_COMMAND_KEYWORD = "jobs";
const string KILL_COMMAND_KEYWORD = "kill";
const string FG_COMMAND_KEYWORD = "fg";
const string BG_COMMAND_KEYWORD = "bg";
const string QUIT_COMMAND_KEYWORD = "quit";
const string CAT_COMMAND_KEYWORD = "cat";


CommandExecutor::CommandExecutor(JobsList &jobsList) : jobsList(jobsList), externalCommand(jobsList) {
    initCommandVariables();
}

CommandExecutor::~CommandExecutor() = default;

void CommandExecutor::executeCommand(const string &commandLine) {
    vector<string> args = parseCommandLine(commandLine.c_str());

    if (!args.empty()) {
        string commandArg = args[0];

        if (this->builtinCommandsArgToCommand.find(commandArg) != this->builtinCommandsArgToCommand.end()) {
            unique_ptr<Command> &command = this->builtinCommandsArgToCommand.at(args[0]);

            if (command != nullptr) {
                // Erase first element which is the command itself
                args.erase(args.begin());

                command->execute(args);
            }
        } else {
            // In this case we did not implement this command so we will try to execute it as an external command
            this->externalCommand.execute(args);
        }
    }
}

string CommandExecutor::ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string CommandExecutor::rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string CommandExecutor::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

vector<string> CommandExecutor::parseCommandLine(const char *cmd_line) {
    vector<string> args;

    std::istringstream iss(trim(string(cmd_line)));

    for (std::string s; iss >> s;) {
        args.emplace_back(s);
    }

    return args;
}

void CommandExecutor::initCommandVariables() {
    unique_ptr<Command> chromptCommand =
            unique_ptr<ChpromptCommand>(new ChpromptCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(CHROMPT_COMMAND_KEYWORD,
                                                                         move(chromptCommand)));

    unique_ptr<Command> lsCommand =
            unique_ptr<LsCommand>(new LsCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(LS_COMMAND_KEYWORD,
                                                                         move(lsCommand)));

    unique_ptr<Command> showPidCommand =
            unique_ptr<ShowPidCommand>(new ShowPidCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(SHOW_PID_COMMAND_KEYWORD,
                                                                         move(showPidCommand)));

    unique_ptr<Command> getCurrentDirectoryCommand =
            unique_ptr<GetCurrentDirectoryCommand>(new GetCurrentDirectoryCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(GET_CURRENT_DIRECTORY_COMMAND_KEYWORD,
                                                                         move(getCurrentDirectoryCommand)));

    unique_ptr<Command> changeDirCommand =
            unique_ptr<ChangeDirCommand>(new ChangeDirCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(CD_COMMAND_KEYWORD,
                                                                         move(changeDirCommand)));

    unique_ptr<Command> jobsCommand =
            unique_ptr<JobsCommand>(new JobsCommand(this->jobsList));
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(JOBS_COMMAND_KEYWORD,
                                                                         move(jobsCommand)));

    unique_ptr<Command> killCommand =
            unique_ptr<KillCommand>(new KillCommand(this->jobsList));
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(KILL_COMMAND_KEYWORD,
                                                                         move(killCommand)));

    unique_ptr<ForegroundCommand> fgCommand =
            unique_ptr<ForegroundCommand>(new ForegroundCommand(this->jobsList));
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(FG_COMMAND_KEYWORD,
                                                                         move(fgCommand)));

    unique_ptr<BackgroundCommand> bgCommand =
            unique_ptr<BackgroundCommand>(new BackgroundCommand(this->jobsList));
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(BG_COMMAND_KEYWORD,
                                                                         move(bgCommand)));

    unique_ptr<QuitCommand> quitCommand =
            unique_ptr<QuitCommand>(new QuitCommand(this->jobsList));
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(QUIT_COMMAND_KEYWORD,
                                                                         move(quitCommand)));

    unique_ptr<CatCommand> catCommand =
            unique_ptr<CatCommand>(new CatCommand());
    builtinCommandsArgToCommand.insert(pair<string, unique_ptr<Command>>(CAT_COMMAND_KEYWORD,
                                                                         move(catCommand)));
}
