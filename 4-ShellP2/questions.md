1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork to create a child process. This allows the program to replace the child rather than the main shell process. If we didn't call fork and called execvp directly, the shell itself would be replaced, which would then terminate the shell session.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, -1 is returned which means that the new process wasn't able to be created. My implmenetation handles this scenario by exiting with ERR_EXEC_CMD. As mentioned in the previous question, forking is necessary to keep the shell running, which is why I made the program exit if the forking fails.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() finds the command to execute by searching for the executable file in the directories in the PATH environment variable. This variable contains a list of directories where executables are stored. When execvp() is called, it iterates throgugh these directories to find the desired command.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() is called to ensure that the parent doesn't go on until the child process is completed. This ensures that the child process isn't orphanzed, meaning that the parent process would end before the child process does. If we didn't call wait(), the child process would potentially be orphaned and would use unnecessary system resources because the child process isn't ending when it is supposed to.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit status of the child process from the status value which is returned by wait(). This provides the return code of the child process, which is important because it indicates whether the command executed successfully or if an error was encountered. This information is important because it can dictate what the parent does next based off this return code.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() handles quoted arguments by toggling a flag called in_quotes whenether quotes are encountered. Inside the quotes, the spaces are preserved and the content is treated as a single argument. Once the closing quote is found, the string is null-terminated and the argument is added to the argv array. Handling quoted arguments like this is necessary to ensure that the information inside of the quotes is copied exactly. This also ensures that if spaces are inside of the quotes, they are handled properly. If we didn't handle quotes like this, spaces inside of quotes may be misinterpreted as multiple commands.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I refactored the parsing logic to focus on a single cmd_buff type instead of a list of commands (command_list_t) like the previous assignment. I also had to modify the logic to handle quoted strings, since this wasn't something I had to worry about in the previous assignment. One unexpected challenge in refactoring my code was the overall handling of the quotes. When I first tried to implement this, I was failing the test cases for spaces within quotes due to copying the string into argv incorrectly. I was able to fix this by adding a toggle variable to handle if I was currently in quotes.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system are used to communicate and notify a process of a certain event or condition. Signals are lightweight and designed to interrupt otherwise normal flow within a process to be able to handle urgent events such as potential termination. Unlike other forms of IPC, signals don't involve data transfer, but rather act like flags/notifications that will trigger an action that is predefined. Signals are also typically faster than other IPCs.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (signal kill) forces the immediate termination of a process. it also can't be caught or ignored and is used when a process has to terminate abruptly. This is used when a process needs to immediately end. SIGTERM (signal terminate) is a signal used to request the termination of a process. Unlike SIGKILL, SIGTERM can be ignored, allowing for a less immediate termination. SIGTERM is used when a process needs to stop, but is less urgent than SIGKILL as SIGTERM gives the process time to clean up resources. SIGINT (signal interrupt) is used to interrupt a process, but not necessarily end it. An example of this is when the user presses CTRL+C.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is immediately paused/stopped by the operating system. It can't be caught or ignored like SIGINT because it is designed to suspend execution until a SIGCONT signal is received. The point of SIGSTOP is to be a reliable way to pause processes and do things like debug.
