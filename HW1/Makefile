#TODO: replace ID with your own IDS, for example: 123456789_123456789
SUBMITTERS := 315836569_203839733
COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
SRCS := Commands.cpp smash.cpp BackgroundCommand.cpp BuiltInCommand.cpp CatCommand.cpp ChangeDirCommand.cpp \
ChpromptCommand.cpp CommandExecutor.cpp CommandsHistory.cpp ExternalCommand.cpp ForegroundCommand.cpp \
GetCurrentDirectoryCommand.cpp JobsCommand.cpp JobsList.cpp KillCommand.cpp PipeCommand.cpp \
QuitCommand.cpp RedirectionCommand.cpp ShowPidCommand.cpp SmallShell.cpp

OBJS=$(subst .cpp,.o,$(SRCS))
HDRS := Commands.h BackgroundCommand.h BuiltInCommand.h CatCommand.h ChangeDirCommand.h ChpromptCommand.h \
CommandExecutor.h CommandsHistory.h ExternalCommand.h ForegroundCommand.h GetCurrentDirectoryCommand.h JobsCommand.h \
JobsList.h KillCommand.h PipeCommand.h QuitCommand.h RedirectionCommand.h ShowPidCommand.h SmallShell.h

TESTS_INPUTS := $(wildcard test_input*.txt)
TESTS_OUTPUTS := $(subst input,output,$(TESTS_INPUTS))
SMASH_BIN := smash

test: $(TESTS_OUTPUTS)

$(TESTS_OUTPUTS): $(SMASH_BIN)
$(TESTS_OUTPUTS): test_output%.txt: test_input%.txt test_expected_output%.txt
	./$(SMASH_BIN) < $(word 1, $^) > $@
	diff $@ $(word 2, $^)
	echo $(word 1, $^) ++PASSED++

$(SMASH_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

zip: $(SRCS) $(HDRS)
	zip $(SUBMITTERS).zip $^ submitters.txt Makefile

clean:
	rm -rf $(SMASH_BIN) $(OBJS) $(TESTS_OUTPUTS) 
	rm -rf $(SUBMITTERS).zip
