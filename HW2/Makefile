TESTS := $(sort $(wildcard test*.cxx))
SOURCES := $(filter-out $(TESTS),$(wildcard *.cxx))
HEADERS := $(wildcard *.h)
TARGETS := $(TESTS:.cxx=.exe)
RESULTS := $(TESTS:.cxx=.out)
GRADE := grade.txt
CXXFLAGS := -std=c++11 -O3

.PHONY: all test clean
all: $(TARGETS)

$(TARGETS): %.exe: %.cxx $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $< $(SOURCES) -o $@

test: $(GRADE)

$(GRADE): $(RESULTS)
	grep SUCCESS $^ | wc -l > $@

$(RESULTS): %.out: %.exe
	taskset -c 0 ./$< > $@ 2>&1

zip:
	zip -r final.zip arch/x86/entry/syscalls/syscall_64.tbl include/linux/init_task.h include/linux/sched.h \
			include/linux/syscalls.h kernel/Makefile kernel/hw2.c submitters.txt

clean:
	rm -f $(TARGETS) $(RESULTS) $(GRADE)
