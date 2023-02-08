#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int main() {
    pid_t pid;
    cout << "I am a parent process" << endl;

    for (int i = 0; i < 4; i++) {
        pid = fork();
        if (pid == 0) {
            cout << "I am a child process" << i << endl;
            for (int x = 3 - i; x >= 0; x--) {
                pid = fork();
                if (pid == 0) {
                    cout << "I am a grandchild process from process" << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            _exit(0);
        }
        wait(nullptr);
    }

    return 0;
}