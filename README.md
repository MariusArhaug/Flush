# Flush

Famous Little Unix Shell

# Description

This repo contains a simple unix based shell _Flush_ based on the course `TDT4186 Operating Systems`

# Getting started

## Dependencies

This project was made with POSIX in mind and therefore is only supported for POSIX systems.

It uses both Lex and Yacc for tokenizing and parsing user input into respective commands and system calls.

## Installing

### Flex and Bison

Flex and Bison are the respective programs to let us use Lex and Yacc in our makefile.

On Linux based systems with the apt package-manager do the following:

```sh
$ sudo apt install flex
```

> Installs Flex to allow use to use Lex.

```sh
$ sudo apt install bison
```

> Installs Bison to allow us to use Yacc.

```sh
$ git clone git@github.com:MariusArhaug/Flush.git
```

```sh
$ cd Flush
```

# Executing shell

# Build

```
$ make
```

> Builds output binaries

```sh
$ make run
```

> Runs the shell, to exit press Ctrl + D

<br >

# Tasks

**3.1) Basic functionality**

When the shell is launched a basic prompt using **yyparse(3)** will appear before the user that accept user input.

This input will be first tokenized based on the RegEx defined in the **scanner.l** file and then based on the grammar defined in the **parse.y** file will result in the right action based on input.

Whenever a user enters a command the _flush_ shell will look witihn the /bin/ directory for that executable. Using the **execute(3)** function defined in **main.c** the main process is forked and and execv call is made in the child process while the parent waits for it to either finish sucessfully or exit with a failure flag.

<br >

**3.2) Changing directories**

Using the **chdir(2)** we can implememnt our own **cd** internal command.
It behaves as intended with the same extra functionality of:

- cd ~
- cd -
- cd

That redirects the shell's current workking directory to $HOME if the param is "~" or " ". And it redirects the shells' cwd to its previous cwd if the param is "-"

<br >

**3.3) IO redirection**

Using the dup2(2) C function we can change a processes stdout and stdin filedescriptor to other specified file descriptors.

These file descriptors are found with the open(2) command that opens a file from a specified path from after a ">" or "<" symbol in the terminal

When doing forexample

```sh
/home/user/flush: ls > /tmp/foo
```

- The program finds the file desc. for /tmp/foo
- Replaces the process **ls'** stdout desc to the file's file desc.
- The output of **ls** now goes into the file instead of stdout.
- Finally we close the necessarry file desc.

<br >

**3.4) Background tasks**

The grammar in the **parser.y** file is set up such that every command call with or without I/O redirection can be ran in the background by terminating the user input with the "&" symbol.

To achieve background processes the **execute(3)** function in the **main.c** accepts two types of flags **WT** (wait) and **BG** (background)

Previously in 3.1 the flag is set to **WT** meaning that the parent process will wait until the child process is finished. If the flag however is set to **BG** the parent process will not wait but instead store the child's PID into a linked list, called defind in the **jobs.c** file.

Before every new prompt the main process will check these nodes in the linked list and see if any of them have terminated, if a job is terminated it can be removed from the linked list and the main process shell will print out to indicate this.

<br >

**3.5) Background task status**

When entering the **jobs** command to the shell it will print out all the the current background processes that are running, if none are running, nothing will be outputed.

<br >

**3.6) Pipelines**

Using a recursive defintion in the **parser.y** a pipeline grammar can also be implemented to support piping output from one process to the next. This is only inteded for external commands, (i.e in /bin/) to avoid any undefined behaviours.

The parser will append every command, arguments and io redirections into a string list that the program will execute after the user has entered in the whole command.

Using the **execute_c(3)** in **main.c** we can create pipes if more than one command was entered in the user input.

There will be a total of n-1 pipes if the user entered n commands. So these pipes needs to be generated and updated so that the next command in the string list will use the previous stdout as its stdin. except for the edge cases of first and last command which dont need their in/out to be changed respectively.

Example

```sh
/home/user/flush: ls -l | grep root | cat > /tmp/file
```
