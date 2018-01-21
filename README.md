# Pintos
Pintos project developing.
Pintos is a simple operating system framework for the 80x86 architecture. It supports kernel
threads, loading and running user programs, and a file system, but it implements all of these in a
very simple way by Stanford Sniversity. You can read more at there [official website](http://web.stanford.edu/class/cs140/projects/pintos/pintos_1.html#SEC1).

## Get started:
You can read instructions to start with pintos form [this link](https://drive.google.com/open?id=1dhuX93CvdsnNU3NsV6cZgpYulnm93F1D).

## Phase #01:
In phase #01, busy waiting in timer had been removed. Also priority scheduler was implemnted to choose max priority first and if there exist one thread or more with the same priority, they are choosen with round robin order.
Also multi level feedback queue scheduler was implemented as an advanced scheduler, You can use it with the flag _-mlfqs_.

## Phase #02:
In phase #02, Userprograms can be run. Kernel memory space has been protected from all invalid pointers and malicious  memory accessing. Also System Calls have been implemented.


## Contributers:
1. [Nada Ayman](https://github.com/N-Einstein) 
2. [Youssef Ahmed](https://github.com/youssef-ahmed)
