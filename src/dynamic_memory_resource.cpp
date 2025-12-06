#include "../include/dynamic_memory_resource.h"
#include <stdexcept>
#include <utility>
#include <iostream>
#include <algorithm>

DynamicMemoryResource::DynamicMemoryResource(
    std::pmr::memory_resource* upstream_resource) noexcept
    : upstream(upstream_resource ? upstream_resource 
               : std::pmr::get_default_resource()),
      total_allocated_(0),
      reused_blocks_(0) {}

DynamicMemoryResource::~DynamicMemoryResource() {
    cleanup();
}

void* DynamicMemoryResource::do_allocate(size_t bytes, size_t alignment) {
    void* ptr = try_reuse_block(bytes, alignment);
    
    if (ptr) {
        reused_blocks_++;
        allocated_blocks[ptr] = bytes;
        return ptr;
    }
    
    ptr = upstream->allocate(bytes, alignment);
    allocated_blocks[ptr] = bytes;
    total_allocated_ += bytes;
    
    return ptr;
}

void DynamicMemoryResource::do_deallocate(void* ptr, size_t bytes, size_t alignment) {
    if (!ptr) return;
    
    auto it = allocated_blocks.find(ptr);
    if (it != allocated_blocks.end()) {
        free_blocks.emplace(bytes, ptr);
        allocated_blocks.erase(it);
    } else {
        bool found_in_free = false;
        auto range = free_blocks.equal_range(bytes);
        for (auto free_it = range.first; free_it != range.second; ++free_it) {
            if (free_it->second == ptr) {
                free_blocks.erase(free_it);
                found_in_free = true;
                break;
            }
        }
        
        if (!found_in_free) {
            throw std::invalid_argument("Deallocating unknown pointer");
        }
    }
}

bool DynamicMemoryResource::do_is_equal(
    const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}

void* DynamicMemoryResource::try_reuse_block(size_t bytes, size_t alignment) {
    auto it = free_blocks.find(bytes);
    if (it != free_blocks.end()) {
        void* ptr = it->second;
        free_blocks.erase(it);
        return ptr;
    }
    
    for (auto larger_it = free_blocks.begin(); larger_it != free_blocks.end(); ++larger_it) {
        if (larger_it->first >= bytes) {
            void* ptr = larger_it->second;
            free_blocks.erase(larger_it);
            return ptr;
        }
    }
    
    return nullptr;
}

void DynamicMemoryResource::cleanup() noexcept {
    for (const auto& [ptr, size] : allocated_blocks) {
        upstream->deallocate(ptr, size, alignof(std::max_align_t));
    }
    
    for (const auto& [size, ptr] : free_blocks) {
        upstream->deallocate(ptr, size, alignof(std::max_align_t));
    }
    
    allocated_blocks.clear();
    free_blocks.clear();
}

size_t DynamicMemoryResource::allocated_count() const noexcept {
    return allocated_blocks.size();
}

size_t DynamicMemoryResource::total_allocated_size() const noexcept {
    return total_allocated_;
}

size_t DynamicMemoryResource::reused_blocks_count() const noexcept {
    return reused_blocks_;
}

void DynamicMemoryResource::dump_statistics() const {
    std::cout << "DynamicMemoryResource Statistics:\n";
    std::cout << "  Allocated blocks: " << allocated_count() << "\n";
    std::cout << "  Free blocks in pool: " << free_blocks.size() << "\n";
    std::cout << "  Total allocated size: " << total_allocated_size() << " bytes\n";
    std::cout << "  Reused blocks: " << reused_blocks_count() << "\n";
}