#include <stdio.h>
/* 
 * stdio.h -- this includes the following
 * 
 * fprintf()
 * printf()
 * stderr
 * getchar()
 * perror()
 */
#include <stdlib.h>
/* 
 * stdlib.h -- this includes the following
 * 
 * malloc()
 * realloc()
 * free()
 * exit()
 * execvp()
 * EXIT_SUCCESS
 * EXIT_FAILURE
 */
#include <unistd.h>
/* 
 * unistd.h -- this includes the following
 * 
 * chdir()
 * fork()
 * exec()
 * pid_t
 */
#include <sys/wait.h>
/* 
 * sys/wait.h -- this includes the following
 * 
 * waitpid()
 * Associated Macros ie. WUNTRACED
 */
#include <string.h>
/* 
 * string.h -- this includes the following
 * 
 * strcmp()
 * strtok()
 */

//if using ssize_t for bsh_readline
// #include <sys/types.h>

// // define exit success and fail
// #define EXIT_SUCCESS 0
// #define EXIT_FAILURE 1

// define buffer size for readline
#define BSH_RL_BUFSIZE 1024

// define token buffer size for split line
#define BSH_TOK_BUFSIZE 64
// define token delimiters
#define BSH_TOK_DELIM " \t\r\n\a"

/*
 * Function Declarations for builtin shell commands
 */
int bsh_cd(char **args);
int bsh_help(char **args);
int bsh_exit(char **args);

/*
 * List of builtin shell commands, followed by their functions
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &bsh_cd,
    &bsh_help,
    &bsh_exit};

int bsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
 * Builtin function implementations
 */
int bsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "bsh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("bsh");
        }
    }
    return 1;
}
int bsh_help(char **args)
{
    int i;
    printf("Brice's Shell BSH\n");
    printf("Type the name of the command <args>, and hit enter\n");
    printf("The following funcions are builtin\n");

    for (i = 0; i < bsh_num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use man <command> for information on other programs\n");
    return 1;
}
int bsh_exit(char **args)
{
    return 0;
}

int bsh_execute(char **args)
{
    int i;

    if (args[0] == NULL)
    {
        //empty command was entered
        return 1;
    }

    for (i = 0; i < bsh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return bsh_launch(args);
}

// allow user to input strings and not have it turned into multiple args
char **bsh_splitline(char *line)
{
    int bufsize = BSH_TOK_BUFSIZE;
    int position = 0;

    char **tokens = malloc(bufsize * sizeof(char));
    char *token;

    // if malloc fails on tokens buffersize exit and print error
    if (!tokens)
    {
        fprintf(stderr, "bsh: splitline allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, BSH_TOK_DELIM);

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        {
            bufsize += BSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char));
            if (!tokens)
            {
                fprintf(stderr, "bsh: splitline reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, BSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// Using built in getline
// char *bsh_readline(void)
// {
//     char *line = NULL;
//     ssize_t bufsize = 0; // have getline allocate a buffer for us
//     getline(&line, &bufsize, stdin);
//     return line;
// }

char *bsh_readline(void)
{
    int bufsize = BSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);

    int c;

    // if malloc returns error exit with error
    if (!buffer)
    {
        fprintf(stderr, "bsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // read a character
        c = getchar();

        // if we hit EOF replace it will null character and return
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }

        // continue incrementing position in allocated space
        position++;

        // if we have exceeded the buffer, reallocate
        if (position > bufsize)
        {
            bufsize += BSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            // if realloc fails to allocate, error out and exit
            if (!buffer)
            {
                fprintf(stderr, "bsh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

// setting up launch to fork and exec programs
int bsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // Child Process
        if (execvp(args[0], args) == -1)
        {
            perror("bsh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // error forking
        perror("bsh");
    }
    else
    {
        // Parent process
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

void bsh_loop()
{
    char *line;
    char **args;
    int status;

    do
    {
        printf(">> ");
        line = bsh_readline();
        args = bsh_splitline(line);
        status = bsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    bsh_loop();
    return EXIT_SUCCESS;
}
