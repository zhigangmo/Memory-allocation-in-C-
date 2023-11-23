
#include <iostream>
#include <list>
#include <unistd.h>  // for sbrk

// Struct to represent a record of memory allocation.
struct Allocation {
    std::size_t size;  // The size of the memory chunk in bytes.
    void* space;       // Pointer to the start of the allocated memory chunk.
};

// Global lists to manage memory.
// Lists all the memory chunks currently in use.
std::list<Allocation> occupied_list;

// Lists all the memory chunks that are available for allocation.
std::list<Allocation> free_list;

// Function pointer to specify the allocation strategy.
// This pointer will point to one of the allocation strategies (either first_fit or best_fit).
Allocation (*allocate_strategy)(std::size_t);

// First Fit allocation strategy:
// This strategy searches for the first available chunk that is big enough to satisfy the request.
Allocation first_fit(std::size_t chunk_size) {
    // Iterate through the free_list to find a suitable chunk.
    for (auto it = free_list.begin(); it != free_list.end(); ++it) {
        if (it->size >= chunk_size) {
            // Found a suitable chunk.
            Allocation allocation = *it;
            // Remove this chunk from free_list since it will now be used.
            free_list.erase(it);
            return allocation;  // Return the found chunk.
        }
    }
    // If no suitable chunk is found, indicate failure.
    return {0, nullptr};
}

// Best Fit allocation strategy:
// This strategy searches for the available chunk that fits the request most tightly.
Allocation best_fit(std::size_t chunk_size) {
    auto best_fit_it = free_list.end();
    // Start with the largest possible difference.
    std::size_t smallest_diff = std::numeric_limits<std::size_t>::max();

    // Iterate through the free_list to find the best-fitting chunk.
    for (auto it = free_list.begin(); it != free_list.end(); ++it) {
        std::size_t size_diff = it->size - chunk_size;
        if (size_diff >= 0 && size_diff < smallest_diff) {
            // Update the smallest difference and iterator pointing to the best fit.
            smallest_diff = size_diff;
            best_fit_it = it;
        }
    }
    if (best_fit_it != free_list.end()) {
        // Found a suitable chunk.
        Allocation allocation = *best_fit_it;
        // Remove this chunk from free_list since it will now be used.
        free_list.erase(best_fit_it);
        return allocation;  // Return the found chunk.
    }
    // If no suitable chunk is found, indicate failure.
    return {0, nullptr};
}

// Function to allocate memory based on the chosen strategy.
void* alloc(std::size_t chunk_size) {
    // Use the current strategy to find a suitable memory chunk.
    Allocation allocation = Allocate_strategy(chunk_size);

    // If a suitable chunk was found in free_list.
    if (allocation.space != nullptr) {
        // Add this chunk to occupied_list since it's now in use.
        occupied_list.push_back(allocation);
        return allocation.space;  // Return the start of the memory chunk.
    }

    // If no suitable chunk is found in free_list, request more memory from the OS using sbrk.
    void* new_space = sbrk(chunk_size);

    // Check if sbrk failed.
    if (new_space == (void*)-1) {
        return nullptr;  // Memory allocation failed.
    }

    // Create a new record for the newly allocated memory.
    Allocation new_allocation = {chunk_size, new_space};
    // Add this new chunk to occupied_list since it's now in use.
    occupied_list.push_back(new_allocation);
    return new_space;  // Return the start of the newly allocated memory chunk.
}

int main() {
    // Set the allocation strategy to First Fit.
    allocate_strategy = first_fit;
    // ... rest of your code ...

    // Later, change the allocation strategy to Best Fit.
    allocate_strategy = best_fit;
    // ... rest of your code ...

    return 0;
}
