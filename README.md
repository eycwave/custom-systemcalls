# Operating Systems - Custom System Call Project

This project was prepared to add and test kernel system calls as part of the Operating Systems course. The project includes custom system calls to retrieve, modify, and test specific process information.

## Contents
1. Project Overview
    1.1 [Key Learning Objectives](#key-learning-objectives)  
    1.2 [Background Information](#background-information)  
    1.3 [System Calls in this Project](#system-calls-in-this-project)  
3. [File Structure](#file-structure)  
4. [Adding System Calls to the Kernel](#adding-system-calls-to-the-kernel)  
5. [Testing](#testing)

## Project Overview

### Key Learning Objectives:

This project involves understanding the following concepts related to system calls:
1. Copying data from **Kernel Space** to **User Space**.
2. Copying data from **User Space** to **Kernel Space**.
3. Understanding the data structures used on the **Kernel** side.
4. Modifying kernel information and learning about how protection & security mechanisms in the kernel can be altered.

### Background Information (Kernel Data Structures):

#### task_struct Structure
- In Linux, information about each process (program) is stored in a structure called `task_struct`.
- This structure contains important details like the process ID (PID) and the state of the process.
- It is defined in the `<linux/sched.h>` header.

```c
struct task_struct {
    pid_t pid;                  // Process ID
    unsigned int __state;       // Current state of the process (TASK_RUNNING, TASK_INTERRUPTIBLE, etc.)
    ...
};
```

#### Process States (__state)
- The state of a process is represented using specific constants in the kernel. Each state has a numerical value associated with it. Below are some of the key process states:

```c
#define TASK_RUNNING        0x00000000  // Process is running
#define TASK_INTERRUPTIBLE  0x00000001  // Process is interruptible (sleeping)
#define TASK_UNINTERRUPTIBLE 0x00000002 // Process is uninterruptible (cannot be stopped)
#define __TASK_STOPPED      0x00000004  // Process is stopped
#define TASK_DEAD           0x00000080  // Process is dead
```

#### Iterating Over All Processes
- In Linux, the `for_each_process()` macro is used to iterate over all the processes in the system.
- This macro loops through each process one by one.
- During the loop, you can print the PID and state of each process.

Here’s an example:

```c
struct task_struct *task;
for_each_process(task) {
    // For each process, 'task' points to the process's task structure
    printf("Process PID: %d, State: %lu\n", task->pid, task->__state);
}
```

#### Returning the Process State as a String
- The `task_state_array[]` converts process state numbers into readable strings (e.g., "R (running)" for TASK_RUNNING).

Here’s an example:

```c
static const char * const task_state_array[] = {
    "R (running)",   // 0x00
    "S (sleeping)",  // 0x01
    "D (disk sleep)",// 0x02
    "T (stopped)",   // 0x04
    "Z (zombie)"     // 0x20
};

// To convert the state of a process to a string:
const char *state_str = task_state_array[task_state_index(task)];
printf("Process State: %s\n", state_str);
```

#### Reading the Current Process State
- If you want to retrieve the state of the currently running process, you can use the `get_current_state()` macro.
- This macro returns the state of the process that is currently being executed (referred to as the "current" process).

Here’s an example:

```c
#define get_current_state() READ_ONCE(current->__state)

unsigned int state = get_current_state();
```

### System Calls in this Project

The following system calls will be implemented in this project:

- `set_proc_state(pid_t pid, int state)`:
  - This system call modifies the state of the process with the given PID.
  - If PID is 0, it changes the state of the calling process (current process) and calls `schedule()` to reschedule the process.

- `get_proc_state(pid_t pid)`:
  - This system call returns the state of the process with the given PID in hexadecimal format.
  - If PID is 0, it returns the state of the calling process.

- `get_proc_state_string(pid_t pid, void *buf, size_t size)`:
  - This system call returns the state of the process with the given PID as a string and copies it to the user space.
  - The `copy_to_user()` function is used to transfer data from the kernel to the user.

- `name_syscall(char *msg)`:
  - This is a custom system call added for demonstration purposes.
  - It prints a message from the user space into the kernel logs.
  - The `strncpy_from_user()` function is used to copy data from user space to the kernel.

## File Structure

The following files have been modified or added:

- `include/linux/syscalls.h`: The prototypes of the new system calls are added in this file.
  
- `kernel/sys.c`: The implementations of the new system calls are defined in this file.
  
- `arch/x86/entry/syscalls/syscall_64.tbl`: The table where the new system call numbers for the x86_64 architecture are defined.
  
- `tests/test_name_syscall.c`: C file used to test the custom system call `name_syscall`.
  
- `tests/test_get_state.c`: C file used to test the custom system calls `get_proc_state` and `get_proc_state_string`.
  
- `tests/test_set_state.c`: C file used to test the system call `set_proc_state`.

## Adding System Calls to the Kernel

To add the system calls to the kernel, follow these steps:

1. Navigate to the kernel source directory (this should be the custom kernel source you have downloaded, not the system's default kernel):
    ```bash
    cd /path/to/linux-<version>
    ```

2. Add the system call prototypes to *syscalls.h*:
    ```bash
    nano include/linux/syscalls.h
    ```

3. Add the following lines to define the prototypes for the new system calls:
    ```c
    asmlinkage long sys_set_proc_state(pid_t pid, int state);
    asmlinkage long sys_get_proc_state(pid_t pid);
    asmlinkage long sys_get_proc_state_string(pid_t pid, void *buf, size_t size);
    asmlinkage long sys_name(char *msg);
    ```

4. Add the system call definitions at the end of the *sys.c* file (outside of any `#if` blocks):
    ```bash
    nano kernel/sys.c
    ```

5. Implement the system calls in this file:
    - `set_proc_state`: This system call is used to modify the state of a process.
    - `get_proc_state`: This system call retrieves the state of a process.
    - `get_proc_state_string`: This system call retrieves the state of a process as a string and copies it to user space.
    - `name_syscall`: A custom system call for demonstration that prints a message from user space to kernel logs.

6. Add the system call numbers to `syscall_64.tbl`:
    ```bash
    nano arch/x86/entry/syscalls/syscall_64.tbl
    ```

7. Add the following lines to assign numbers to the new system calls:
    ```bash
    551    common  sys_set_proc_state         sys_set_proc_state
    552    common  sys_get_proc_state         sys_get_proc_state
    553    common  sys_get_proc_state_string  sys_get_proc_state_string
    554    common  sys_name                   sys_name
    ```

8. Compile and install the kernel:
    ```bash
    make -j$(nproc)  # This command compiles the kernel using multiple CPU cores in parallel.
    sudo make modules_install  # This installs the kernel modules (drivers).
    sudo make install  # This installs the newly compiled kernel into the boot directory.
    sudo update-grub  # This updates the GRUB bootloader configuration.
    sudo reboot  # Reboot the system to load the new kernel.
    ```

## Testing

To test the new system calls, follow the steps below:

### Test `test_name_syscall.c`:
This test checks the functionality of the custom system call *name_syscall*. It sends a message from the user space to the kernel logs.
   
    ```bash
    gcc test_name_syscall.c -o test_syscall
    ./test_syscall
    sudo dmesg | tail -10  # Check the kernel logs for the message
    ```

### Test `test_get_state.c`:
This test is used to verify the functionality of *get_proc_state* and *get_proc_state_string* system calls. These calls retrieve the state of a process, either in hexadecimal or string format.
    
    ```bash
    gcc test_get_state.c -o test_get_state
    ./test_get_state [PID]  # Replace [PID] with the process ID you want to test
    sudo dmesg | tail -10  # Check the kernel logs for the output
    ```

### Test `test_set_state.c`:
This test is used to check the functionality of the *set_proc_state* system call, which modifies the state of a process. It changes the state of the process with the given PID to a specified value.
    
    ```bash
    gcc test_set_state.c -o test_set_state
    ./test_set_state [PID] 0x01  # Replace [PID] with the process ID, and 0x01 with the desired state
    sudo dmesg | tail -10  # Check the kernel logs for confirmation
    ```
