project1: threads/ , devices/timer <little modif.>, 
project2: userprog/ , lib/ <include and little modif.> , examples/
project3: vm/
-------------------
dont use strcpy, strncpy, strcat, strncat, strok, sprintf, vsprintf
-------------------
debugging and testing:
--------
1- printf()
2- ASSERT <macro>
3- debug_backtrace() in <debug.h>
4- GDB debugger
------------------
project1 steps:
1- read threads folder
	idle() >> runs if no threads in ready or running queue
	thread_create() acts like main()
	switch.s <assemply code> indicates context switch mechanism
	switch_threads() --You will understand the thread system once you understand why and how the switch_threads() 
		that gets called is different from the switch_threads() that returns.--


2- source files: '-*' <not important to look at> , '+*' < should look and modify > , '**' < important and must to modify>
	** thread.c , _.h >> Basic thread support.
	+* init.c , _.h >> initialize and contain main
	+* synch.c, _.h >> Basic synchronization primitives: semaphores, locks, condition variables, and optimization barriers. You will need to use these for synchronization in all four projects. 
	* start.S >> Does basic setup needed for memory protection and 32-bit operation on 80x86 CPUs. Unlike the loader, this code is actually part of the kernel
	* switch.s, _.h
	* malloc.h , _.c >> simple implementation of malloc() and free()
	* interrupt.c , _.h >> simple implementation for interrupt handling
	* intr-stubs.S, _.h >> Assembly code for low-level interrupt handling.
	-* kernel.lds.S >> linker script to link the kernel.
	-* loader.h, _.S >> The kernel loader. Assembles to 512 bytes of code and data that the PC BIOS loads into memory and which in turn finds the kernel on disk, loads it into memory, and jumps to start() in start.S
	-* io.h >> Functions for I/O port access. This is mostly used by source code in the devices directory
	-* for phase 3 , vaddr.h , pte.h >> Functions and macros for working with virtual addresses and page table entries. These will be more important to you in project 3
	-* flags.h >> Macros that define a few bits in the 80x86 "flags" register. Probably of no interest

3- devices code: '-*' <not important to look at> , '+*' < should look and modify > , '**' < important and must to modify>
	+* timer.c , _.h >> System timer that ticks, by default, 100 times per second. You will modify this code in this project.
	* ide.c , _.h >> Supports reading and writing sectors on up to 4 IDE disks.
	* partition.c , _.h >> Understands the structure of partitions on disks, allowing a single disk to be carved up into multiple regions (partitions) for independent use.
	* kbd.c >> Keyboard driver. Handles keystrokes passing them to the input layer
	* input.c >> input layer. Queues input characters passed along by the keyboard or serial drivers.
	* intq.c >> nterrupt queue, for managing a circular queue that both kernel threads and interrupt handlers want to access. Used by the keyboard and serial drivers.
	* rtc.c >> Real-time clock driver, to enable the kernel to determine the current date and time. By default, this is only used by thread/init.c to choose an initial seed for the random number generator.
	* speaker.c >> Driver that can produce tones on the PC speaker.
	* pit.c >> Code to configure the 8254 Programmable Interrupt Timer. This code is used by both devices/timer.c and devices/speaker.c because each device uses one of the PITs output channel.
	-* vga.c , _.h >> VGA display driver. Responsible for writing text to the screen. You should have no need to look at this code. printf() calls into the VGA display driver for you
	-* serial.c , _.h >> Serial port driver.
	-* for phase 2 , block.c , _.h >> An abstraction layer for block devices, that is, random-access, disk-like devices that are organized as arrays of fixed-size blocks.

4- lib files: '-*' <not important to look at> , '+*' < should look and modify > , '**' < important and must to modify>
	+* kernel/list.c >> Doubly linked list implementation.
	+* kernel/bitmap.c >> Bitmap implementation
	+* kernel/hash.c >> Hash implementation
	+* kernel/console.c , stdio.h >> Implements printf() and a few other functions.
	* round.h >> Macros for rounding
	* random.c >> Pseudo-random number generator. The actual sequence of random values will not vary from one Pintos run to another, unless you do one of three things: specify a new random seed value on the -rs kernel command-line option on each run, or use a simulator other than Bochs, or specify the -r option to pintos.
	* debug.c >> Functions and macros to aid debugging. 

5- Synchronization: 
	- Any synchronization problem can be easily solved by turning interrupts off: while interrupts are off, there is no concurrency, so theres no possibility for race conditions
	- use semaphores, locks, and condition variables to solve the bulk of your synchronization problems.
	- the only class of problem best solved by disabling interrupts is coordinating data shared between a kernel thread and an interrupt handler. Because interrupt handlers can't sleep, they can't acquire locks.
	- There should be no busy waiting in your submission. A tight loop that calls thread_yield() is one form of busy waiting.
	- We should use 'git'.

---------------------------------------------------------------

REQUIREMENTS:
1- Design Document
	in pintos/src/threads/DESIGNDOC
2- Alarm clock
	Reimplement timer_sleep(), defined in devices/timer.c
	It "busy waits," that is, it spins in a loop checking the current time and calling thread_yield() until enough time has gone by. 
	Reimplement it to avoid busy waiting.
	// see more in page
3- Priority Scheduling
	Implement priority scheduling in Pintos.
	When a thread is added to the ready list that has a higher priority than the currently running thread, the current thread should immediately yield the processor to the new thread.
	Similarly, when threads are waiting for a lock, semaphore, or condition variable, the highest priority waiting thread should be awakened first.
	A thread may raise or lower its own priority at any time, but lowering its priority such that it no longer has the highest priority must cause it to immediately yield the CPU.
	// see more in page
4-  Advanced Scheduler
	Implement a multilevel feedback queue scheduler to reduce the average response time for running jobs on your system. 
	// see more in page
---------------------------------------------------------------

Esitmated number of changes: 5 files changed, 440 insertions(+), 29 deletions(-)

-----------------------------------------------------------------
Section:
- alarm clock >> if sleep(2 seconds)>> sleep at least 2 seconds , >> if thread should wake now -global_time()- or prevoiusly then wake it
- Synchronization primitives need to be modified (from busy waiting -polling- to disable interrupts) #start with it
- lock is mutex or binary semaphore < with queue >
- priority donation -inheritence- < note: multidonation , more than one thread with higher priority need the lock >
- if ready queue is changed then call schedulers
- make check >> run all tests in the system
- We need to make header for float operations >> for example * 10 then / 10 at last -make 10 greater for more accuracy = (2^14)-
	 We cannt use float as 8086 dont understand floats
	 Implement: add , sub , mul = x*y/2^14 , div = x*2^14/y , real_to_int(real x) = x/2^14, int_to_real(int x) = x*2^14
- We have 64 queue < 64 level from 0 to 63 > ... Really we have 1 queue contains threads with its own priority ... loop and find higher priority and run it
	 we can do this by loop reversly and make var called max .. if thread.priority >= max then take it ,, we make >=  not > only to find the first thread with higher priority
	 we check on priority as int not real to make it round robin per queue
	 Summary: we use round robin for each queue and start with queue with higher priority
- Each 4 ticks compute priority equation: priority = PRI_MAX - (recent_cpu / 4) - (nice * 2) <for all ready threasds>
	recent_cpu >> for each tick>> running thread.recent_cpu++ < very greater than nice >
		for each second>> all threads including running.recent_cpu = (2*load_avg)/(2*load_avg + 1) * recent_cpu + nice
			hint: there is a critical section >> as running thread can change by decreasing every second and increasing every tick <use mutex not interrupts disable>
			for all system: load_avg <global> >> estimates the average number of threads ready to run over the past minute.
				load_avg = load_avg = (59/60)*load_avg + (1/60)*ready_threads
	nice >> how thread is nice from -20 <not nice> to 20 <very nice>
	PRI_MAX >> maximum value for priority
--------------------------------------------------
Not understand:
---------------
thread switching >> last 3 points ,, where is switch_threads() implementation
#sema_down >> why did he use while in it 
sema_up >> should call scheduler !
#timer_sleep() >> not blocked while looping
priority queue starvation ?
2 same priority threads >> take 1 or switching
nested waiting complexity #???
need to check mlfqs # yes we need to check it

-----------------
Nada >> priority , synchronization
Y Ahmed >> priority donation , timer
Y Zook >> BSD , mlfqs , real , git
