#define _POSIX_SOURCE // kill warning
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define LENGTH 100

int fd1[2];
int fd2[2];
char filename[LENGTH];

// CPU registers
int pc = -1, sp = 1000, ir = -1, ac = 0, x = 0, y = 0, timer = 0;

// values for initial reference -- sspE == system stack pointer end
int flag = 0, sspE = 1999;

void checker(int n, int flag)
{
    if ((n > 999) && (flag == 0))
    {
        printf("Memory violation: accessing system address %d in user mode\n", n);
        signal(SIGCHLD, SIG_IGN);
        kill(getpid(), SIGCHLD);
        exit(6);
    }
}

int handler(int *pc, int *timer, int *call)
{

    // general registers for handler
    int n = 0;
    int temp = 0;
    int sig = -1;

    // timer handler if timer is greater than call (timer value given in command line)
    if (*timer >= *call)
    {

        // if not in user mode reset timer
        if (*pc < 1000)
        {
            *timer = 0;
            flag = 1;
        }

        // save sp in system stack
        if (*pc <= 1499)
        {

            // save sp in stack
            write(fd1[1], &sig, sizeof(int)); // send signal to memory for writing
            write(fd1[1], &sspE, sizeof(int));
            write(fd1[1], &sp, sizeof(int));

            // save sp to current value of the stack system
            sp = sspE;
            *pc -= 1; // bcs incremented in the end
            sp -= 1;  // decresed to save pc in correct spot
            // save pc in system stack
            write(fd1[1], &sig, sizeof(int));
            write(fd1[1], &sp, sizeof(int));
            write(fd1[1], pc, sizeof(int));
            *pc = 1000;
        }
    }

    // read instruction
    write(fd1[1], pc, sizeof(int));
    read(fd2[0], &ir, sizeof(int));

    // checking
    // printf("pc = %d, IR = %d, Ac = %d, X = %d, Y = %d, Sp = %d, timer = %d, Call = %d, flag= %d\n",*pc ,ir, ac, x, y , sp, *timer, *call, flag);

    switch (ir)
    {
    case 1:
        *pc += 1;
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        checker(n, flag);
        ac = n;
        break;
    case 2:
        *pc += 1;
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int)); // address to read next from mem
        // user/kernel check
        checker(n, flag);
        write(fd1[1], &n, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        break;
    case 3:
        *pc += 1;
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int)); // address to read next from mem
        // user/kernel check
        checker(n, flag);

        write(fd1[1], &n, sizeof(int));
        read(fd2[0], &n, sizeof(int)); // address to read next from mem

        // user/kernel check
        checker(n, flag);

        write(fd1[1], &n, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        break;
    case 4:
        *pc += 1;
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        checker(n, flag);
        ac = n + x;
        checker(ac, flag);
        write(fd1[1], &ac, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        break;
    case 5:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));

        ac = n + y;
        // user/kernel check
        checker(ac, flag);
        write(fd1[1], &ac, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        break;
    case 6:
        temp = sp + x;
        // user/kernel check
        checker(temp, flag);
        write(fd1[1], &temp, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        break;
    case 7:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        write(fd1[1], &sig, sizeof(int));
        write(fd1[1], &n, sizeof(int));
        write(fd1[1], &ac, sizeof(int));
        break;
    case 8:
        ac = rand() % 100 + 1;
        break;
    case 9:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        if (n == 1)
        {
            printf("%d", ac);
        }
        else
        {
            printf("%c", (char)ac);
        }
        break;
    case 10:
        ac += x;
        break;
    case 11:
        ac += y;
        break;
    case 12:
        ac -= x;
        break;
    case 13:
        ac -= y;
        break;
    case 14:
        x = ac;
        break;
    case 15:
        ac = x;
        break;
    case 16:
        y = ac;
        break;
    case 17:
        ac = y;
        break;
    case 18:
        sp = ac;
        break;
    case 19:
        ac = sp;
        break;
    case 20:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], pc, sizeof(int));
        *pc -= 1;
        break;
    case 21:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        if (ac == 0)
        {
            *pc = n - 1; // incremented in the end
        }
        break;
    case 22:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        if (ac != 0)
        {
            *pc = n - 1; // incremented in the end
        }
        break;
    case 23:
        *pc += 1;
        // user/kernel check
        checker(*pc, flag);
        write(fd1[1], pc, sizeof(int));
        read(fd2[0], &n, sizeof(int));
        write(fd1[1], &sig, sizeof(int)); // signal to write in the stack
        sp -= 1;
        write(fd1[1], &sp, sizeof(int));
        *pc += 1;                       // incremented to match next instruction
        write(fd1[1], pc, sizeof(int)); // push return value
        *pc = n - 1;                    // jump to address incremented in the end
        break;
    case 24:
        // user/kernel check
        checker(sp, flag);
        write(fd1[1], &sp, sizeof(int));
        read(fd2[0], pc, sizeof(int));
        sp += 1;
        *pc -= 1; // since bcs later increased
        break;
    case 25:
        x += 1;
        break;
    case 26:
        x -= 1;
        break;
    case 27:
        write(fd1[1], &sig, sizeof(int));
        sp -= 1;
        write(fd1[1], &sp, sizeof(int));
        write(fd1[1], &ac, sizeof(int));
        break;
    case 28:
        // user/kernel check
        checker(sp, flag);
        write(fd1[1], &sp, sizeof(int));
        read(fd2[0], &ac, sizeof(int));
        sp += 1;
        break;
    case 29:
        write(fd1[1], &sig, sizeof(int));
        write(fd1[1], &sspE, sizeof(int));
        write(fd1[1], &sp, sizeof(int));
        sp = sspE;
        sp -= 1;
        write(fd1[1], &sig, sizeof(int));
        write(fd1[1], &sp, sizeof(int));
        write(fd1[1], pc, sizeof(int));
        *pc = 1499;
        flag = 1;
        break;
    case 30:
        // user/kernel check
        checker(sp, flag);
        write(fd1[1], &sp, sizeof(int));
        read(fd2[0], pc, sizeof(int));
        sp += 1;
        write(fd1[1], &sp, sizeof(int));
        read(fd2[0], &sp, sizeof(int));
        flag = 0;
        break;
    case 50:
        signal(SIGCHLD, SIG_IGN);
        kill(getpid(), SIGCHLD);
        return 0;
        break;
    default:
        printf("ERROR CASES");
        return 0;
        break;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    // check if input valid
    if (argc <= 2)
    {
        printf("ERROR TERMINAL LINE");
        exit(0);
    }

    // check if timer is valid
    if (atoi(argv[2]) == 0)
    {
        printf("ERROR TERMINAL LINE TIMER NOT VALID");
        exit(1);
    }

    int call = atoi(argv[2]);
    int tmp = 0;
    // copy argv to string to pass in write
    if (strlen(argv[0]) >= LENGTH)
        fprintf(stderr, "%s is too long!\n");
    else
        strcpy(filename, argv[1]);

    // add termination value to string
    filename[strlen(filename)] = '\0';

    // save length of string filename
    int n = strlen(filename) + 1;

    // random value
    srand(time(NULL) + getpid());

    if (pipe(fd1) < 0 || pipe(fd2) < 0)
    {
        write(STDERR_FILENO, "Pipe failed\n", 12);
        exit(2);
    }

    switch (fork())
    {
    case -1:
        write(STDERR_FILENO, "Fork failed\n", 12);
        exit(3);

    case 0:
        dup2(fd1[0], STDIN_FILENO);
        dup2(fd2[1], STDOUT_FILENO);
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        execl("memory", "memory", NULL);
        write(STDERR_FILENO, "Exec failed\n", 12);
        exit(4);

    default:
        // send signal to memory read file
        write(fd1[1], &pc, sizeof(int));
        pc += 1;
        write(fd1[1], &n, sizeof(int));

        // send string with filename to child
        if (write(fd1[1], filename, strlen(filename) + 1) < 0)
            return 1;

        read(fd2[0], &tmp, sizeof(int));

        // error file handler
        if (tmp == 1)
            exit(5);

        // read the amount of instruction the program has to process
        int handResult = 1;
        while (handResult)
        {
            handResult = handler(&pc, &timer, &call);
            pc++;
            timer += 1;
        }
    }
    return 0;
}
