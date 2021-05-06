//
// Created by amit and roy on 01/05/21.
//

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include "CatCommand.h"

using std::cout;
using std::endl;

void CatCommand::execute(vector<string> args) {
    vector<string> filenames = args;

    if (args.empty()) {
        cerr <<"smash error: cat: not enough arguments" << endl;
    } else {
        for (const auto &currentFilename : filenames) {
            printFile(currentFilename);
        }
    }
}

void CatCommand::printFile(const string &filename) const {
    int fd = open(filename.c_str(), O_RDONLY, 0666);

    if (fd == -1) {
        perror("smash error: open failed");
    } else {
        ssize_t bytesRead;
        char buffer[DEFAULT_BLOCK_SIZE + 1];
        bool shouldKeepReading = true;

        while (shouldKeepReading) {
            bytesRead = read(fd, buffer, DEFAULT_BLOCK_SIZE);

            if (bytesRead == -1) {
                perror("smash error: read failed");
                return;
            } else {
                buffer[bytesRead] = '\0';
                cout << buffer;

                shouldKeepReading = bytesRead > 0;
            }
        }

        cout << flush;

        bool didNotClose = close(fd) == -1;

        if (didNotClose) {
            perror("smash error: close failed");
        }
    }
}
