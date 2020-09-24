# P-Machine
Made by Weiyi Chen (cwy.sophiechen@gmail.com) and Alice Zhang (alicezhang32092@knights.ucf.edu) for COP3402

This assignment implements a VM P-machine (PM/0) to execute p-code.
The P-machine is a stack machine with two memory segments: a data-stack that contains the data segment and a stack to be used by the PM/0 CPU, and the instructions  for the VM to execute. The PM/0 CPU has four registers to handle the stack and text segments: The registers are named base pointer (BP), stack pointer (SP), program  counter (PC) and instruction register (IR). 

# How to use

```
1. p-machine:
  - Open to Terminal/Command Prompt
  - type "gcc main.c" to complile
  - type "./a.out input.txt" to run
  
```
