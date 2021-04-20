//
// Created by amit and shvmo on 11/15/20.
//

#include "LsCommand.h"
#include <dirent.h>
#include "SmallShell.h"
#include <iostream>
#include <unistd.h>

LsCommand::LsCommand() = default;

LsCommand::~LsCommand() = default;

void LsCommand::execute(vector<string> args) {
    // Declaring array to call a c function that deals with sorting directories
    dirent **direntNameList;

    SmallShell &shell = SmallShell::getInstance();

    int directoriesNum =
            scandir(shell.getCurrentDirectory().c_str(), &direntNameList, nullptr, alphasort);

    if (directoriesNum == -1) {
        perror("smash error: scandir failed");
    } else {
        dirent **dirnetIterator = direntNameList;

        // skip ".", ".." dirs
        ++dirnetIterator;
        ++dirnetIterator;

        for (int i = 2; i < directoriesNum; ++i, ++dirnetIterator) {
            cout << (*dirnetIterator)->d_name << endl;
        }

    }
}
