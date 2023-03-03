Kevin Monahan
CompSci 537 - Homework 3
2/15/2023

This is simplified version of bash. This shell can take in any command so long as the path has been set properly. This shell supports unlimited
whitespace between commands, arguments, and the other modifiers used in the shell. Not everything needs whitespace, but when it is needed it 
is noted. The shell also includes three built-in functions. They are "cd", "exit", and "path".

cd
The cd command takes in one argument separated from the main command by at least one whitespace and attempts to switching the current working
directory to the specified path. Here is an example -> "cd   .." This will fail is the path is non-viable or more than one argument is passed

exit
The exit command takes in no arguments and closes the shell safely. If arguments are included, the shell will not exit. 

path
The path command allows the user to pass in paths to the shell to use programs from that location. This shell does not support absolute paths
supplied to the shell or relative paths. The path command supports three different arguments. If these arguments are not passed to the function
the path command will note an error. The three arguments supported are "add", "clear", and "remove". These arguments cannot be passed
to the command at once and will cause errors. The arguments need to be at least one whitespace away from the command, but more is fine.
On initialization of the shell the path "/bin" is added to the path list

    add - Allows the user to add any path to the list of paths. The viability of the path is not checked when it is added. The path being added
    must be at least one whitespace away from the argument. 

    clear - Removes all paths from the list of paths. If no paths are added after this only the built-in commands will work in the shell

    remove - Removes a specified path from the list of paths. The path being removed must be at least one whitespace away from the argument.
    If the path supplied to be removed does not match any of the paths in the list an error will be thrown.

This shell also supports running multiple commands in parallel. It should be noted that built in commands are not supported. In order to
run multiple commands in parallel separate the commands with a "&". Any arguments related to the commands should be on the same side of
the "&". If one side, or both sides, of the "&" does not contain a command no errors will be thrown. 

This shell also supports running commands after the completion of other commands. This is done using the ";" character between the commands.
Any arguments related to the commands should be on the same side of the ";". If one side, or both sides, of the ";" does not contain a 
command no errors will be thrown. This does work with built in commands and works with the "&" character.

This shells also supports placing the output of commands in a file. This is done by using the ">" character. This character should come after
all arguments related to the command. After the ">" character only one argument is accepted. If an argument is not given or more than one is
given an error will be thrown. If the file specified by the argument after the ">" does not exist, it will be created. If the file does exist
all information in that file will be erased and replaced by the output of the command. Any error messages related to the command will also be
placed in the file. However, if syntactical errors exist the output file will not be used and instead the error will be placed in the terminal.
It should be noted that built in commands do not work with this functionality. 

An example of something that results in an error message is "ls; pdd > out" This line will not execute ls even though it is syntactically correct
because pdd does not exist and the entire line fails.

An example of something that does work in the shell is "cd          ..; ls               -al > text.txt &    pwd; path    add    /h;    exit" 

All errors that are syntactical in nature or are related to the built-in commands will return "An error has occurred"

This shell can also be used in batch mode. In this mode when the shell is called an argument is also given. If more than one argument is given
the shell will exit. If the file given does not exist, the shell will also exist. In batch mode the shell does not print out a prompt to take in
commands. It instead parses the given file and attempts to execute the commands within. The argument must be at least one whitespace away from 
the command. Like in normal operating mode if there is an error in a line the entire line fails. However, the shell will then continue to parse 
the file and execute commands.

In order to use this shell simply call "make" then "./smash". This will put you in normal mode. For their use the shell as normal noting the
restrictions laid out above. If a command you believe should work is not working, make sure to add a path to the location of the command and try
again. If you want to use the batch mode when calling "./smash" include a file with it. Such as "./smash test.txt"

Because of how the shell parses entire lines of commands at once, if you are attempting to run a program from the shell that needs a path specified or
needs to be compiled, this must be done a separate line above the command that you want to use. If you try to set a path and then run a command on that
path in the same line it will produce an error. 