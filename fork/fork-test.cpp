#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
    cout << "I am the parent process" << endl;
    int pid;
    for (int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == 0) {
            cout << "I am the child process " << i << endl;
            if (i == 1) {
                pid = fork();
                if (pid == 0) {
                    cout << "I am the grandchild process from child process " << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            sleep(5);
            _exit(0);
        }
        wait(nullptr);
    }
    // for (int i = 0; i < 3; i++)
    //     wait(nullptr);

    return 0;
}