# PURPOSE:  This program finds the maximum number of a
#           set of data items.
#
# VARIABLES: The registers have the following uses:
#
# %edi - Holds the index of the data item being examined
#          Destination Index register
# %ebx - Largest data item found
#          Base register
# %eax - Current data item
#          Accumulator register
#
# The following memory locations are used:
#
# data_items - contains the item data.  A 0 is used
#              to terminate the data
# BUILDING:
#          as maximum.s -o maximum.o
#          ld maximum.o -o maximum

.section .data
data_items:
 .long 3,67,34,222,45,75,54,34,44,33,22,11,66,253,0

.section .text
 .globl _start
 _start:
 movl $0, %edi                   # move 0 into the index register
 movl data_items, %eax           # load the first byte of data
 movl %eax, %ebx                 # since this is the first item,
                                 # %eax is the biggest

start_loop:
 cmpl $0, %eax                   # check to see if we've hit the end
 je loop_exit                    # jump to exit if we have
 incl %edi                       # increment index
 movl data_items(,%edi,4), %eax  # load next value
 cmpl %ebx, %eax                 # compare to current largest
 jle start_loop                  # jump to loop beginning if the new
                                 # one isn’t bigger (eflags (status) register)
 movl %eax, %ebx                 # update largest value
 jmp start_loop                  # jump to loop beginning

loop_exit:
 # %ebx is the status code for the exit system call
 # and it already has the maximum number
 movl $1, %eax             #1 is the exit() syscall
 int  $0x80
