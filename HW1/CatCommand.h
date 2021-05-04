//
// Created by amit and roy on 01/05/21.
//

#ifndef OPERATINGSYSTEMWHW_CATCOMMAND_H
#define OPERATINGSYSTEMWHW_CATCOMMAND_H

#include "BuiltInCommand.h"

class CatCommand : public BuiltInCommand {
public:
    CatCommand() = default;

    ~CatCommand() = default;

    void execute(vector<string> args) override;

    const int DEFAULT_BLOCK_SIZE = 4096;

    void printFile(const string &filename) const;
};


#endif //OPERATINGSYSTEMWHW_CATCOMMAND_H
