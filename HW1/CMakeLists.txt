cmake_minimum_required(VERSION 3.15)
project(OSWet1)

set(CMAKE_CXX_STANDARD 11)

add_executable(OSWet1
        Commands.cpp Commands.h
        smash.cpp
        SmallShell.cpp SmallShell.h
        BuiltInCommand.cpp BuiltInCommand.h
        ExternalCommand.cpp ExternalCommand.h
        PipeCommand.cpp PipeCommand.h
        RedirectionCommand.cpp RedirectionCommand.h
        ChangeDirCommand.cpp ChangeDirCommand.h
        GetCurrentDirectoryCommand.cpp GetCurrentDirectoryCommand.h
        ShowPidCommand.cpp ShowPidCommand.h
        QuitCommand.cpp QuitCommand.h
        CommandsHistory.cpp CommandsHistory.h
        JobsList.cpp JobsList.h
        JobsCommand.cpp JobsCommand.h
        KillCommand.cpp KillCommand.h
        ForegroundCommand.cpp ForegroundCommand.h
        BackgroundCommand.cpp BackgroundCommand.h
        ChpromptCommand.cpp ChpromptCommand.h
        LsCommand.cpp LsCommand.h
        CommandExecutor.cpp CommandExecutor.h
        CatCommand.cpp CatCommand.h)