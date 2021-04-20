//
// Created by amit and shvmo on 11/15/20.
//

#include "ChpromptCommand.h"
#include "SmallShell.h"


using namespace std;

const string DEFAULT_PROPMTNAME = "smash";

ChpromptCommand::ChpromptCommand() = default;

void ChpromptCommand::execute(vector<string> args) {
    if (!args.empty()) {
        SmallShell::getInstance().setCommandPromptName(args[0]);
    } else {
        SmallShell::getInstance().setCommandPromptName(DEFAULT_PROPMTNAME);
    }
}

ChpromptCommand::~ChpromptCommand() = default;
