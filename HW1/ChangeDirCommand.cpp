//
// Created by shvmo and amit on 13/11/2020.
//

#include "ChangeDirCommand.h"
#include "SmallShell.h"
#include <unistd.h>
#include <iostream>


const int MAX_NUM_OF_ARGS = 1;
const char SPECIAL_ARG_TO_CHANGE_TO_PREV_DIRECTORY[2] = "-";

ChangeDirCommand::ChangeDirCommand() = default;

ChangeDirCommand::~ChangeDirCommand() = default;

void ChangeDirCommand::execute(vector<string> args) {
    SmallShell &shell = SmallShell::getInstance();
    int result;
    string newDirectory;
    string previousDirectory = shell.getCurrentDirectory();

    // todo: make sure if needs to be included or deleted
    //if (args.empty()) {
    //    cout << "smash error: kill: invalid arguments" << endl;
    //
    //    return;
    //} else
    if (args.size() > MAX_NUM_OF_ARGS) {
        cout << "smash error: cd: too many arguments" << endl;

        return;
    } else if (args[0] == SPECIAL_ARG_TO_CHANGE_TO_PREV_DIRECTORY) {
        if (shell.getPreviousDirectory().empty()) {
            cout << "smash error: cd: OLDPWD not set" << endl;

            return;
        } else {
            result = chdir(shell.getPreviousDirectory().c_str());

            newDirectory = shell.getPreviousDirectory();
        }
    } else {
        result = chdir(args[0].c_str());
        newDirectory = args[0];
    }

    if (result == -1) {
        perror("smash error: chdir failed");
    } else {
        char *currentDir = get_current_dir_name();

        if (currentDir == nullptr) {
            perror("smash error: get_current_dir_name failed");
        } else {
            // easier to get current_dir_name because sometimes we need to concatenate paths
            shell.setCurrentDirectory(currentDir);
            shell.setPreviousDirectory(previousDirectory);

            free(currentDir);
        }
    }
}
