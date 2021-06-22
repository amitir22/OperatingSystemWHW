#include <stdlib.h>
#include <stdio.h>

using namespace std;

// todo:
int run_tests1() {
    return 0;
}

// todo:
int run_tests2() {
    return 0;
}

// todo:
int run_tests3() {
    return 0;
}

// todo:
int run_tests4() {
    return 0;
}

int run_all_tests(){
    return run_tests1() | run_tests2() | run_tests3() | run_tests4();
}

int main(int argc, char *argv[]) {
    int test_num = -1;

    if (argc == 1) {
        return run_all_tests();
    } else if (argc > 2) {
        return -1;
    } else { // argc == 2
        test_num = atoi(argv[1]);

        switch (test_num) {
            case 1:
                return run_tests1();
            case 2:
                return run_tests2();
            case 3:
                return run_tests3();
            case 4:
                return run_tests4();
            default:
                cout << "invalid arguments. correct usage: tests <test-num: 1/2/3/4>" << endl;
                return -1;
        }
    }
}