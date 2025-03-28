1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    Fork allows us to keep a child process without terminating the parent process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    If fork() fails, the implementation handles the error and prints an error message, but the shell continues to run along with all other processes.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    execvp() finds the command from a list of all commands via the PATH environment variable. PATH contains all directories that house possible commands.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    wait() stops child processes from being established until parent process is completed execution. Without this zombie processes could form which are child processes with no parent.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    It provides exit status of a child process. This is important because it allows for better error handling control when deciding how to handle child process unexpected or expected termination.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    It correctly treats all text within quotes as a single argument. This is necessary in order to control field separators when providing arguments or commands with spaces in the name. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    The main issue was changing how the tokenization of arguments was handled. Because spacing and proper separator control was necessary for this implementation I had account for that by stripping for quotes and detecting newline instead of just whitespace. 

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    Signals operate asynchronously to linux processes making them useful for detecting external interrupts from hardware or software devices like ports.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    SigKILL acts as a last resort for process termination when other more elementary methods fail. SigTERM is similar but it doesnt act immediately it instead gives the process time to perform environment cleanup memory freeing etc. SigINT or ctrl + c, is an instant interrupt that stops the process during whatever its doing allowing for new process to be done in its place.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    SigSTOP tells the kernel to immediately stop all processes in place. Because it is handled directly by the kernel, there is no way to override it using traps within a shell.
  
