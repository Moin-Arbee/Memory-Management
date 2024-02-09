# Memory-Management
The Memory Management simulates memory allocation and deallocation in a constrained environment with a fixed total memory size (given via input). The program processes transactions to dynamically allocate and deallocate memory blocks, periodically optimising memory usage through compaction.

Features:
Dynamic Memory Allocation: The program allows dynamic allocation of memory blocks using a mechanism similar to interpreted languages like Python or Java.

Memory Deallocation: Memory blocks can be deallocated when they are no longer needed. The reference count mechanism ensures that deallocation only occurs when the reference count reaches zero.

Periodic Memory Compaction: To mitigate memory fragmentation, the system performs periodic memory compaction after a specified number of deallocations. This optimization ensures efficient memory utilisation over time.

Reference Counting: The assignment introduces reference counting for memory blocks. Assignments like a=b increase the reference count of the memory previously allocated to a. This mechanism helps manage shared memory resources.

Command-Line Arguments:
The program accepts command-line arguments to configure its behaviour:
./YourProgramName <totalMemorySize> <defragmentationInterval>
totalMemorySize: Specifies the total size of the available memory (e.g., 64 MB).
defragmentationInterval: Sets the interval for periodic memory compaction. The system performs compaction after every k deallocations, where k is the provided value.


Assumptions:
Total Memory Size: The program assumes a fixed total memory size as given in the Command-Line Arguments.
Smallest Addressable Unit: The program assumes that a byte is the smallest addressable unit of memory. All memory operations are performed at the byte level.
Memory Array Representation: The memory is represented using a binary array named memoryArray. It is implemented as a vector of integers (vector<int>), where each index corresponds to a byte of memory. The values in this array are binary: 0 indicates unallocated memory, and 1 indicates allocated memory.
Initialization of MemoryArray: Initially, all indices of the memoryArray are set to 0, signifying unallocated memory. As memory is allocated or deallocated, the corresponding indices are updated accordingly.
Allocation and Deallocation Representation: The allocation of memory is depicted by setting the values of corresponding indices in memoryArray to 1. Conversely, deallocation involves resetting these indices to 0.
Periodic Defragmentation: The program implements periodic defragmentation after every k deallocations, where k is a parameter provided through command-line arguments. This decision is made to balance the overhead of defragmentation and ensure an efficient allocation process.
Defragmentation is not performed after every deallocation. Instead, it is scheduled after a certain number of deallocations (k). This approach optimises the average time complexity, as defragmentation is more cost-effective when there is noticeable fragmentation in memory.
The cost of defragmentation is considered to be O(n), where n is the total number of allocated memory blocks. By performing defragmentation periodically, the average cost is distributed over a series of operations, minimising its impact on each allocation or deallocation.
If memory allocation fails due to insufficient available memory, an attempt to defragment the memory is made. If successful, the memory allocation is retried. This approach aims to optimise memory usage and fulfil allocation requests whenever possible.


Data Structures Used:
The Memory Management system utilises several data structures to efficiently manage and track the state of memory. These data structures are fundamental to the allocation, deallocation, and referencing operations performed by the program.

Memory Array (vector<int> memoryArray):
Purpose: Represents the memory space as an array of binary values.
Use:
Each index of the array corresponds to a byte of memory.
Elements are initialised to 0 indicating unallocated memory.
Values of 1 denote allocated memory.
Provides a binary representation of the memory state.

Allocated Blocks (list<MemoryBlock> allocatedBlocks):
Purpose: Stores information about allocated memory blocks.
Use:
Each MemoryBlock object contains details such as start address, size, reference count, and associated variables.
Tracks memory blocks that are currently in use.
Allows for easy traversal and management of allocated memory.

Free Blocks (list<FreeBlock> freeBlocks):
Purpose: Manages information about unallocated memory blocks.
Use:
Each FreeBlock object records the start address and size of a free memory block.
Maintains a list of memory blocks that are available for allocation.
Supports efficient allocation by identifying contiguous free blocks.

Variable to Address Map (unordered_map<string, int> variableToAddress):
Purpose: Maps variable names to their respective memory block start addresses.
Use:
Provides a quick lookup mechanism to find the starting address of memory associated with a variable.
Facilitates efficient referencing and deallocation operations.
Helps in maintaining a connection between variables and memory addresses.


Transactions
Transactions in the Memory Management system represent the operations performed by the program to allocate, reference, and deallocate memory. Each transaction is a command provided in the input file, guiding the program on how to interact with the memory system.

Allocate Transaction:
Command: allocate <size> <variable>
Example: allocate 25 a
Description: Allocates a block of memory with the specified size for the variable a. The allocated block is assigned a unique starting address, and the corresponding memory indices are marked as allocated (1) in the memory array.

Reference Transaction:
Command: reference <DestinationVariable> <sourceVariable>
Example: reference b a
Description: This operation means that the destination variable (b) now refers to the same memory block as the source variable (a), and the reference count of that memory block increases by 1.

Free Transaction:
Command: free <variable>
Example: free a
Description: Decreases the reference count of the memory block associated with the variable a. If the reference count of the memory block drops to zero, the block is marked as unallocated (0) in the memory array.

Print Transaction:
Command: print
Description: Outputs the current state of the memory, including information about allocated and free memory blocks. It provides insights into the memory structure after a sequence of transactions.

