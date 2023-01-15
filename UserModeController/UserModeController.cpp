#include <iostream>
#include "Communicator.h"
#include <stdio.h>
#include <Windows.h>

struct options
{
    bool help = false;
    bool show_this_pid = true;
    bool hide_this_pid = false;
    bool hide_smth = false;
    ULONG pid = 3300;
};

void parse_options(int argc, char** argv, options* opts);

int main(int argc, char** argv)
{
    options opts;
    parse_options(argc, argv, &opts);
    if (opts.help)
    {
        printf("Possible arguments:\n");
        printf("\t-h => help\n");
        printf("\t-st => do not show this app's pid        default=%d\n", !opts.show_this_pid);
        printf("\t-ht => hide this app's pid               default=%d\n", opts.hide_this_pid);
        printf("\t-p <ULONG> => hide this pid\n");
        return 0;
    }

    Communicator cmtor = Communicator("\\\\.\\hidedriver");

    DWORD pid = opts.pid;
    
    DWORD this_pid = GetCurrentProcessId();

    if (opts.show_this_pid)
        printf("This process id=%d\n", this_pid);

    if (opts.hide_this_pid)
    {
        if (cmtor.hideTask(this_pid))
            printf("Done.\n");
        else
            printf("Failed to hide process [PID=%d]\n", this_pid);
    }

    if (opts.hide_smth)
    {
        if (cmtor.hideTask(pid))
            printf("Done.\n");
        else
            printf("Failed to hide process [PID=%d]\n", pid);
    }

    //Sleep(10000);
    system("pause");
    return 0;
}

void parse_options(int argc, char** argv, options* opts)
{
    if (argc < 2)
    {
        printf("Default options (use -h for help)\n");
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            if (std::strcmp(argv[i], "-h") == 0)
                opts->help = true;

            else if (std::strcmp(argv[i], "-st") == 0)
                opts->show_this_pid = false;

            else if (std::strcmp(argv[i], "-ht") == 0)
            {
                //opts->hide_smth = true;
                opts->hide_this_pid = true;
            }
            else if (std::strcmp(argv[i], "-p") == 0)
            {
                opts->hide_smth = true;
                opts->pid = std::atoi(argv[i + 1]);
            }
        }
    }
}
