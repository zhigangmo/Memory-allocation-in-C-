#include <limits> 
#include <iostream>
#include <list>
#include <unistd.h>  // for sbrk

// Struct to represent a memory allocation
struct Allocation {
    std::size_t size;
    void* space;
};

// Global lists to manage memory
std::list<Allocation> occupied_list;
std::list<Allocation> free_list;

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

// New deallocation function
void dealloc(void* ptr) {
    for (auto it = occupied_list.begin(); it != occupied_list.end(); ++it) {
        if (it->space == ptr) {
            free_list.push_back(*it);
            occupied_list.erase(it);
            return;
        }
    }
    // If we reach here, it means the pointer wasn't found in occupied_list.
    // This is an error scenario in this simple implementation.
}

int main() {
    allocate_strategy = first_fit;  // Set strategy to First Fit

    void* mem1 = alloc(1000);
    std::cout << "Allocated 1000 bytes at " << mem1 << std::endl;

    dealloc(mem1);
    std::cout << "Deallocated memory at " << mem1 << std::endl;

    allocate_strategy = best_fit;  // Switch strategy to Best Fit

    void* mem2 = alloc(500);
    std::cout << "Allocated 500 bytes at " << mem2 << std::endl;

    dealloc(mem2);
    std::cout << "Deallocated memory at " << mem2 << std::endl;

    return 0;
}
