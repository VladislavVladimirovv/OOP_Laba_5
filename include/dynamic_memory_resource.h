#ifndef DYNAMIC_MEMORY_RESOURCE_H
#define DYNAMIC_MEMORY_RESOURCE_H

#include <memory_resource>
#include <map>
#include <unordered_map>
#include <list>
#include <cstddef>

class DynamicMemoryResource : public std::pmr::memory_resource {
private:
    std::map<void*, size_t> allocated_blocks;
    
    std::unordered_multimap<size_t, void*> free_blocks;
    
    std::pmr::memory_resource* upstream;
    size_t total_allocated_;
    size_t reused_blocks_;
    
public:
    explicit DynamicMemoryResource(
        std::pmr::memory_resource* upstream_resource = 
            std::pmr::get_default_resource()) noexcept;
    
    DynamicMemoryResource(const DynamicMemoryResource&) = delete;
    DynamicMemoryResource& operator=(const DynamicMemoryResource&) = delete;
    
    ~DynamicMemoryResource() override;
    
    size_t allocated_count() const noexcept;
    size_t total_allocated_size() const noexcept;
    size_t reused_blocks_count() const noexcept;
    
    void dump_statistics() const;
    
protected:
    void* do_allocate(size_t bytes, size_t alignment) override;
    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;
    
private:
    void* try_reuse_block(size_t bytes, size_t alignment);
    void cleanup() noexcept;
};

#endif