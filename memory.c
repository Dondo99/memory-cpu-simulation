#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define LENGTH 100

int main()
{

    // memory registers + array memory
    int counter = 0;
    int n, index = -1, temp = 0;
    int mem[2000];

    // every cell of array = -1 for error checking
    for (int i = 0; i < 2000; i++)
        mem[i] = -1;

    // file check
    int flag = 0;
    char filename[LENGTH];
    char line[MAX_LINE_LENGTH];

    // check if need to read file name
    read(0, &index, sizeof(int));

    if (index < 0)
    {
        // read file name and size
        read(0, &n, sizeof(int));
        read(0, &filename, sizeof(char) * n);
        ;
        // open file
        FILE *textfile = fopen(filename, "r");
        if (textfile == NULL)
        {
            fprintf(stderr, "%s", "ERROR FILE\n");
            temp = 1;
            // send erro file value
            write(1, &temp, sizeof(int));
            exit(0);
        }

        // send ok file value
        write(1, &temp, sizeof(int));

        // get values for memory
        while (fgets(line, MAX_LINE_LENGTH, textfile))
        {
            char *p = line;
            while (*p)
            { // While there are more characters to process
                // finish to read exit the while
                if (*p == ' ')
                    break;
                if (*p == '.')
                {
                    flag = 1;
                }
                if (isdigit(*p) || ((*p == '-' || *p == '+') && isdigit(*(p + 1))))
                {
                    // Found a number
                    long val = strtol(p, &p, 10); // read value
                    if (flag == 1)
                    {
                        counter = val;
                        flag = 0;
                    }
                    else
                    {
                        mem[counter++] = val; // store
                    }
                }
                else
                {
                    // move next
                    p++;
                }
            }
        }
        fclose(textfile);
    }

    // reading/writing logic
    while (1)
    {
        read(0, &index, sizeof(int));
        // writing
        if (index < 0)
        {
            read(0, &index, sizeof(int));
            read(0, &temp, sizeof(int));
            mem[index] = temp; // write address
        }
        else
        { // reading
            write(1, &mem[index], sizeof(int));
        }
    }
    return 0;
}
