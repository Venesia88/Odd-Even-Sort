# Odd-Even-Sort

Rank 4/67 (based on execution time)

# Implement a parallel odd-even sort algorithm using MPI.

Odd-Even sort is a comparison sort that consists of odd-phase and even-phase. In the even phase, all even/odd indexed pairs of adjacent elements are compared. On the other hand, in the odd phase, odd/even indexed pairs are compared. A process can only exchange its local elements with its neighbor process, for example, MPI task with rank 5 can only exchange elements with rank 4 and 6.

## Implementation:
Every process will get at least n / size, $size$ is the total number of processes, while $n$ is the total number of data to sort. The remainder of data will be evenly divided into processes. float_sort from boost library is used for the local sorting. 

Before exchanging the whole data, each process will exchange their maximum or minimum data before exchanging their whole section of data, this reduced the total amount of communication time needed. Next, merge sort is done by swapping array pointer, instead of copying the entire array.

`MPI_Allreduce` and `MPI_LOR` are used to check for early termination. If the current state is the same as the previous state, the program will terminate early.



