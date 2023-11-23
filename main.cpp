
#include <limits>
#include <iostream>
#include <list>
#include <unistd.h>
#include <algorithm>

// Struct to represent a memory allocation
struct Allocation {
    std::size_t size;
    void* space;
};

// Global lists to manage memory
std::list<Allocation> occupied_list;
std::list<Allocation> free_list;

// Global variables for reporting
std::size_t total_allocated = 0;
std::size_t total_freed = 0;
unsigned int num_allocations = 0;
unsigned int num_deallocations = 0;

// Function pointers for allocation strategies
Allocation (*allocate_strategy)(std::size_t);

Allocation first_fit(std::size_t chunk_size) {
    for (auto it = free_list.begin(); it != free_list.end(); ++it) {
        if (it->size >= chunk_size) {
            Allocation allocation = *it;
            free_list.erase(it);
            return allocation;
        }
    }
    return {0, nullptr};  // Indicate failure
}

Allocation best_fit(std::size_t chunk_size) {
    auto best_fit_it = free_list.end();
    std::size_t smallest_diff = std::numeric_limits<std::size_t>::max();
    for (auto it = free_list.begin(); it != free_list.end(); ++it) {
        std::size_t size_diff = it->size - chunk_size;
        if (size_diff >= 0 && size_diff < smallest_diff) {
            smallest_diff = size_diff;
            best_fit_it = it;
        }
    }
    if (best_fit_it != free_list.end()) {
        Allocation allocation = *best_fit_it;
        free_list.erase(best_fit_it);
        return allocation;
    }
    return {0, nullptr};  // Indicate failure
}

void* alloc(std::size_t chunk_size) {
    num_allocations++; // Increment the number of allocations
    total_allocated += chunk_size; // Increase the total allocated memory count

    Allocation allocation = allocate_strategy(chunk_size);
    if (allocation.space != nullptr) {
        occupied_list.push_back(allocation);
        return allocation.space;
    }
    void* new_space = sbrk(chunk_size);
    if (new_space == (void*)-1) {
        return nullptr;  // sbrk failed
    }
    Allocation new_allocation = {chunk_size, new_space};
    occupied_list.push_back(new_allocation);
    return new_space;
}

void dealloc(void* ptr) {
    num_deallocations++; // Increment the number of deallocations

    auto it = std::find_if(occupied_list.begin(), occupied_list.end(), 
                           [ptr](const Allocation& alloc) { return alloc.space == ptr; });

    if (it != occupied_list.end()) {
        total_freed += it->size; // Increase the total freed memory count
        free_list.push_back(*it);
        occupied_list.erase(it);
    } else {
        std::cerr << "Error: Pointer not found in occupied list during deallocation." << std::endl;
    }
}

void report() {
    std::cout << "===== Memory Manager Report =====" << std::endl;
    std::cout << "Total Allocated: " << total_allocated << " bytes" << std::endl;
    std::cout << "Total Freed: " << total_freed << " bytes" << std::endl;
    std::cout << "Number of Allocations: " << num_allocations << std::endl;
    std::cout << "Number of Deallocations: " << num_deallocations << std::endl;
    std::cout << "=================================" << std::endl;
}

int main() {

#ifdef STRATEGY
#if STRATEGY == 1
allocate_strategy = first_fit;
#elif STRATEGY == 2
allocate_strategy = best_fit;
#endif
#endif

    //allocate_strategy = first_fit;  // Set strategy to First Fit
    //allocate_strategy = best_fit;  // Set strategy to First Fit

    std::string command;
    while (std::getline(std::cin, command)) {
        if (command.find("alloc:") != std::string::npos) {
            std::size_t size = std::stoul(command.substr(7));  // extract the size from the command
            void* mem = alloc(size);
            std::cout << "Allocated " << size << " bytes at " << mem << std::endl;
        } else if (command == "dealloc") {
            if (!occupied_list.empty()) {
                void* lastAllocated = occupied_list.back().space;
                dealloc(lastAllocated);
                std::cout << "Deallocated memory at " << lastAllocated << std::endl;
            } else {
                std::cerr << "Warning: Attempted deallocation with no occupied memory." << std::endl;
            }
        }
    }

    report(); // Display the memory manager report at the end
    return 0;
}
