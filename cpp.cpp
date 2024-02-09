#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>
#include <algorithm> 
using namespace std;
struct MemoryBlock 
{
    int start,
        size,
        refCount;
    vector<string> variables; // To store variable names referencing the same memory block
};

struct FreeBlock 
{
    int start,
        size;
};

class MemoryManager 
{
    private:
        const int totalMemory;
        vector<int> memoryArray;
        list<MemoryBlock> allocatedBlocks;
        list<FreeBlock> freeBlocks;
        unordered_map<string, int> variableToAddress;
    public:
        
        /***************************************************************
        Class Constructor: MemoryManager

        Use: Initializes an instance of the MemoryManager class with
        a specified size, creating an initial block of memory
        and setting up the necessary data structures for memory
        management.

        Arguments: size - an integer representing the total size of the
        memory managed by the MemoryManager.

        Returns: Nothing.

        Notes: The constructor initializes the totalMemory attribute the specified size 
        and creates an initial FreeBlock covering the entire memory space. The memoryArray
        is initialized to zeros, indicating that all memory is initially unallocated. This 
        class is designed for managing dynamic memory allocation and deallocation, and it uses
        linked lists to keep track of allocated and free memory blocks. The initial free block 
        covers the entire memory space, and as memory is allocated and deallocated,these linked lists 
        are updated accordingly.
        ***************************************************************/

        MemoryManager(int size) : totalMemory(size), memoryArray(size, 0) 
        {
            FreeBlock initialFreeBlock = {0, size};
            freeBlocks.push_back(initialFreeBlock);
        }


        /***************************************************************
        Function: allocateMemory

        Use: Allocates a block of memory for a given variable.

        Arguments:
        1. size: an integer representing the size of the memory block to be allocated.
        2. variableName: a constant reference to a string representing the name of the 
                         variable for which memory is to be allocated.

        Returns:
        -1: If the variable with the given name already has memory allocated.
        -2: If there is not enough memory available for the requested allocation.
        startAddress: If memory is successfully allocated, returns the starting address of the allocated block.

        Notes:
        The function searches for a free block with sufficient size to accommodate the requested memory, if multiple
        blocks have sufficient size, it allocates the smallest block of memory that is sufficient to accommodate
        the memory request.
        If a suitable block is found, it allocates memory, updates the memory array and free block list,
        and maintains a mapping of the variable name to the allocated memory's starting address.
        ***************************************************************/

        // int allocateMemory(int size, const string& variableName) 
        // {
        //     if (variableToAddress.find(variableName) != variableToAddress.end()) 
        //     {
        //         return -1; 
        //     }
        //     for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) 
        //     {
        //         if (it->size >= size) 
        //         {
        //             int startAddress = it->start;
        //             MemoryBlock newBlock = {startAddress, size, 1, {variableName}};
        //             allocatedBlocks.push_back(newBlock);

        //             // Update memory array to mark as allocated
        //             for (int i = startAddress; i < startAddress + size; ++i) 
        //             {
        //                 memoryArray[i] = 1;
        //             }

        //             // Update free block list
        //             it->start += size;
        //             it->size -= size;
        //             if (it->size == 0) 
        //             {
        //                 freeBlocks.erase(it);
        //             }

        //             // Update variable to address mapping
        //             variableToAddress[variableName] = startAddress;

        //             return startAddress;
        //         }
        //     }

        //     return -2; // Not enough memory
        // }
        int allocateMemory(int size, const string& variableName, auto &outputFile) // used best fit here.
        {
            if (variableName.empty()) 
            {
                outputFile << "Variable name cannot be empty. " << variableName << "\n";
                return -3;
            }
            if (isdigit(variableName[0])) 
            {
                outputFile << "Variable name cannot start with a digit. " << variableName << "\n";
                return -3;
            }
            if (variableToAddress.find(variableName) != variableToAddress.end()) 
            {
                return -1; // Variable already exists
            }

            auto bestFitBlock = freeBlocks.end(); // Iterator to the best fit block
            int minSizeDifference = numeric_limits<int>::max(); // Minimum size difference initialized to maximum

            // Iterate through the free blocks to find the best fit
            for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) 
            {
                if (it->size >= size) 
                {
                    int sizeDifference = it->size - size;

                    // Update the best fit if the current block has a smaller size difference
                    if (sizeDifference < minSizeDifference) 
                    {
                        bestFitBlock = it;
                        minSizeDifference = sizeDifference;
                    }
                }
            }

            // Check if a suitable free block is found
            if (bestFitBlock != freeBlocks.end()) 
            {
                int startAddress = bestFitBlock->start;
                MemoryBlock newBlock = {startAddress, size, 1, {variableName}};
                allocatedBlocks.push_back(newBlock);

                // Update memory array to mark as allocated
                for (int i = startAddress; i < startAddress + size; ++i) 
                {
                    memoryArray[i] = 1;
                }

                // Update free block list
                bestFitBlock->start += size;
                bestFitBlock->size -= size;

                // If the block is fully allocated, remove it from the free block list
                if (bestFitBlock->size == 0) 
                {
                    freeBlocks.erase(bestFitBlock);
                }

                // Update variable to address mapping
                variableToAddress[variableName] = startAddress;

                return startAddress; // Return the start address of the allocated memory block
            }

            return -2; // Not enough memory
        }

        
        /***************************************************************
        Function: deallocateMemory

        Use:       Deallocates memory for a given variable name.

        Arguments: 1. variableName: a constant reference to a string, representing the name of the variable to deallocate.
                   2. outputFile: a reference to an output stream, where error or informational messages are written.

        Returns:   1, if memory deallocation is successful, otherwise just writes the case in the output file itself.

        Notes:     This function searches for the specified variable name in the variableToAddress map. 
                   If found, it decreases the reference count of the associated memory block. If the 
                   reference count becomes zero, the memory is marked as deallocated in the memory array, 
                   and the block is either added to the free block list.
                   If the variable is not found, an error message is written to the output stream. The function 
                   returns 1 on success.
        ***************************************************************/

        
        int deallocateMemory(const string& variableName, auto &outputFile)
        {
            auto it = variableToAddress.find(variableName);
            if (it != variableToAddress.end()) 
            {
                int startAddress = it->second;
                auto blockIt = find_if(allocatedBlocks.begin(), allocatedBlocks.end(),
                                            [startAddress](const MemoryBlock& block) 
                                            {
                                                return block.start == startAddress;
                                            });

                if (blockIt != allocatedBlocks.end()) 
                {
                    // Remove from variable to address mapping
                    variableToAddress.erase(it);
                    blockIt->variables.erase(remove(blockIt->variables.begin(), blockIt->variables.end(), variableName), blockIt->variables.end());   
                    if (--(blockIt->refCount) == 0) 
                    {
                        // Update memory array to mark as deallocated
                        for (int i = startAddress; i < startAddress + blockIt->size; ++i) {
                            memoryArray[i] = 0;
                        }

                        // Update free block list
                        FreeBlock newFreeBlock = {startAddress, blockIt->size};
                        auto freeBlockIt = freeBlocks.begin();
                        while (freeBlockIt != freeBlocks.end() && freeBlockIt->start < newFreeBlock.start) {
                            ++freeBlockIt;
                        }

                        if (freeBlockIt != freeBlocks.begin() && prev(freeBlockIt)->start + prev(freeBlockIt)->size == newFreeBlock.start) {
                            prev(freeBlockIt)->size += newFreeBlock.size;
                        } else {
                            freeBlocks.insert(freeBlockIt, newFreeBlock);
                        }

                        allocatedBlocks.erase(blockIt);
                    }
                    else
                    {
                        outputFile<<"Reference count decreased by one for the block referred by "<<variableName<<endl;
                        return -1;
                    }
                }
            }
            else
            {
                outputFile<<"Error: Variable " << variableName << " is not allocated.\n";
                return -1;
            }
            return 1;
        }

        /***************************************************************
        Function: referenceMemory

        Use:        Associates var1 with the same block of memory as var2,
                    updating the reference count and memory mapping.

        Arguments: 1. var1: a constant reference to a string representing
                    the variable to be associated with var2.
                2. var2: a constant reference to a string representing
                    the variable whose memory block will be referenced.

        Returns:   -1 if var1 is already present in the variableToAddress map
                    (indicating allocation failure due to duplicate variable).
                    true if var1 is successfully associated with the same memory
                    block as var2, and the reference count and memory mapping
                    are updated accordingly.
                    false if there is no block associated with var2.

        Notes:      This function checks if var1 is already present in the
                    variableToAddress map, and returns -1 in such cases. It then
                    searches for the block associated with var2 in the allocated
                    blocks list. If found, var1 is added to the variables list of
                    the same block, the reference count is increased, and the
                    variableToAddress mapping is updated. If no block is found
                    for var2, the function returns false.
        ***************************************************************/
        int referenceMemory(const string& var1, const string& var2)
        {
            // Check if var1 already exists
            if (variableToAddress.find(var1) != variableToAddress.end()) 
            {
                return -1; // var1 already exists
            }

            // Find the block associated with var2
            auto itVar2 = variableToAddress.find(var2);
            if (itVar2 != variableToAddress.end()) 
            {
                int var2StartAddress = itVar2->second;

                // Find the block in allocatedBlocks associated with var2
                auto blockIt = find_if(allocatedBlocks.begin(), allocatedBlocks.end(),
                    [var2StartAddress](const auto& block) { return block.start == var2StartAddress; });

                if (blockIt != allocatedBlocks.end()) 
                {
                    // Add var1 to the variables of the same block
                    blockIt->variables.push_back(var1);
                    // Increase the reference count
                    blockIt->refCount++;

                    // Update variable to address mapping for var1
                    variableToAddress[var1] = var2StartAddress;

                    return true; // Reference successful
                }
            }

            return false; // No block associated with var2
        }

        /***************************************************************
        Function: unfragment
        Use: Performs memory compaction by re-addressing allocated blocks and updating related data structures.
        Arguments: totalMemorySize - an integer representing the total size of the memory (in bytes).
        Returns: Nothing.
        Notes:
        This function sorts the memoryArray in descending order, bringing all allocated blocks (1's) to the start and unallocated blocks (0's) to the end.
        Clears the existing freeBlock list.
        Re-addresses the allocated blocks, updating their starting addresses and the variableToAddress map.
        Inserts any remaining memory as a new free block.
        ***************************************************************/
        void unfragment(int totalMemorySize) 
        {
            // Sort memory array in descending order
            sort(memoryArray.rbegin(), memoryArray.rend());
            //This brings all 1's i.e. allocated blocks to the start and all 0's to the end.

            // Clear existing freeBlock list
            freeBlocks.clear();
            int currentAddress = 0;

            // Re-address the allocated blocks
            for (auto& block : allocatedBlocks) 
            {
                block.start = currentAddress;
                currentAddress += block.size;
                
                for (const auto& variable : block.variables) 
                {
                    variableToAddress[variable] = block.start; // Update variableToAddress map for each variable in the vector
                }
            }

            // Insert the remaining memory as a free block
            if (currentAddress < totalMemorySize) 
            {
                FreeBlock freeBlock;
                freeBlock.start = currentAddress;
                freeBlock.size = totalMemorySize - currentAddress;
                freeBlocks.push_back(freeBlock);
            }
        }
        
        /***************************************************************
        Function: printMemoryState
        Use: Prints the current state of memory, including details
        about allocated and free blocks.
        Arguments: outputFile - a reference to an output stream (modified by printMemoryState).
        The function assumes that the stream is open.
        Returns: nothing
        Notes: This function iterates over the lists of allocated and free blocks
        and prints their details to the specified output stream. It calculates
        and prints the total memory allocated and total memory free.
        ***************************************************************/
        void printMemoryState(auto & outputFile ) 
        {
            if (outputFile.is_open()) 
            {
                int totalfree = 0, totalallocated = 0;
            outputFile << "\n";
            outputFile << "Allocated Blocks:\n";
            for (const auto& block : allocatedBlocks) 
            {
                outputFile << "Start: " << block.start << ", Size: " << block.size << ", RefCount: " << block.refCount << ", Variables: ";
                for (const auto& variable : block.variables) {
                    outputFile << variable << " ";
                }
                outputFile << "\n";

                // Accumulate the total allocated memory size
                totalallocated += block.size;
            }
            outputFile << "Free Blocks:\n";
            for (const auto& block : freeBlocks) 
            {
                outputFile << "Start: " << block.start << ", Size: " << block.size << "\n";

                // Accumulate the total free memory size
                totalfree += block.size;
            }
            outputFile << "Total Memory Allocated: " << totalallocated << " bytes\n";
            outputFile << "Total Memory Free: " << totalfree << " bytes\n\n";
            } 
            else 
            {
                // If unable to open the file, print to standard error
                cerr << "Error opening output file.\n";
            }
        }

};

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        cerr << "You must pass Total Memory Size and Defragmentation Interval.\n";
        return 1;
    }
    int totalMemorySize = stoi(argv[1]);
    int k = stoi(argv[2]);
    if(totalMemorySize < 1)
    {
        cerr << "Total Memory should be a natural number. Considering default case as 100. \n";
        totalMemorySize=100;
    }
    if(k < 1)
    {
        cerr << "The period after which you want the compaction should occur should be a natural number. \n Considering default case as 1";
        k=100;
    }
    int defragmentationInterval=k;
    MemoryManager memoryManager(totalMemorySize);

    ifstream inputFile("C:\\Users\\Arbee\\OneDrive\\Desktop\\Moin\\LP01\\input.txt");
    if (!inputFile.is_open()) 
    {
        cerr << "Error opening input file.\n";
        return 1;
    }

    ofstream outputFile("C:\\Users\\Arbee\\OneDrive\\Desktop\\Moin\\LP01\\output.txt");
    if (!outputFile.is_open()) 
    {
        cerr << "Error opening output file.\n";
        return 1;
    }
    
    string line;
    while (getline(inputFile, line)) 
    {
        istringstream iss(line);
        string transactionType;
        iss >> transactionType;

        if (transactionType == "allocate")
        {
            int size;
            string variableName;
            iss >> size >> variableName;
            int startAddress = memoryManager.allocateMemory(size, variableName, outputFile);
            if (startAddress >= 0 ) 
            {
                outputFile << "Allocated " << size << " bytes for variable " << variableName << " at address " << startAddress << "\n";
            } else if (startAddress == -2)
            {
                outputFile << "Not enough memory for allocation as of now. Trying after eliminating any present fragmentation.\n\n";
                memoryManager.unfragment(totalMemorySize);
                startAddress=memoryManager.allocateMemory(size, variableName, outputFile);
                if (startAddress >= 0 ) 
                {
                    outputFile << "Allocated " << size << " bytes for variable " << variableName << " at address " << startAddress << "\n";
                }
                else
                    outputFile << "Error: Still not enough memory for allocation.\n";
            }
            else if (startAddress == -1)
            {
                outputFile<<"A variable with the same name as '"<< variableName << "' is already present.\nDeallocate it or change the current variable name to something else.\n";
            }
            
        } 
        else if (transactionType == "free") 
        {
            string variableName;
            iss >> variableName;
            int status = memoryManager.deallocateMemory(variableName,outputFile);
            if(status >= 0)
            {
                outputFile << "Deallocated memory for variable " << variableName << "\n";
                defragmentationInterval--;
                if(defragmentationInterval == 0)
                {
                    memoryManager.unfragment(totalMemorySize);
                    defragmentationInterval=k;
                }
            }
        } 
        else if (transactionType == "print") 
        {
            memoryManager.printMemoryState(outputFile);
        }
        else if (transactionType == "reference") 
        {
            string var1, var2;
            iss >> var1 >> var2;
            int flag=memoryManager.referenceMemory(var1, var2);
            if (flag==1) 
            {
                outputFile << "Reference: " << var1 << " is now referring to the same block as " << var2 << "\n";
            } 
            else if(flag==0)
            {
                outputFile << "Error: " << var2 << " does not refer to any block.\n\n";
            } 
            else
            {
                outputFile << "Error: " << var1 << " already refers to a block.\n\n";
            }
        }
        else 
        {
            outputFile << "Error: Unknown transaction type.\n";
        }      
    }
    inputFile.close();
    outputFile.close();
    return 0;
}