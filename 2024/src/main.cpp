#include <chrono>
#include <execinfo.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include "index.cpp"

using namespace std;

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, handler);
    if (argc < 2) {
        cout << "error: no day number passed!\n";
        return 1;
    }

    int day_nr = atoi(argv[1]);
    bool isPart1 = true;
    if (argc > 2) isPart1 = strcmp(argv[2], "b") != 0;

    bool test_file = false;
    if (argc > 3) test_file = strcmp(argv[3], "test") == 0;

    string filename =
        "data/day" + to_string(day_nr) + (test_file ? "_test.txt" : ".txt");
    ifstream file(filename);
    vector<string> lines = vector<string>();
    string curr_line;
    while (getline(file, curr_line)) {
        lines.push_back(curr_line);
    }

    auto begin = std::chrono::steady_clock::now();
    try {
        std::string result = run_day(day_nr, lines, isPart1);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_str =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
                .count();

        cout << "Finished in " << elapsed_str << "ms: " << result << endl;
        // your stuff
    } catch (const std::exception &ex) {
        cerr << ex.what() << endl;
    }

    return 0;
}
