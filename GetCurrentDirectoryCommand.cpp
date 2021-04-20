//
// Created by shvmo and amit on 13/11/2020.
//

#include "GetCurrentDirectoryCommand.h"
#include "SmallShell.h"
#include <iostream>

GetCurrentDirectoryCommand::GetCurrentDirectoryCommand() = default;

GetCurrentDirectoryCommand::~GetCurrentDirectoryCommand() = default;

void GetCurrentDirectoryCommand::execute(vector<string> args) {
    cout << SmallShell::getInstance().getCurrentDirectory() << endl;
}
