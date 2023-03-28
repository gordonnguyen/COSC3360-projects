#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    cout << "I am the parent process" << endl;
    int pid;

    for(int x = 0; x < 4;x++){
        pid = fork();
        if(pid == 0){
            cout << "I am the child process " << x << endl;
            if(x == 1 || x == 2){
                pid = fork();
                if(pid == 0){
                    cout << "I am the grandchild process from child process " << x << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            _exit(0);
        }
        wait(nullptr);
    }
}