#include <gtest/gtest.h>
#include "dynamic_memory_resource.h"
#include <vector>
#include <string>

TEST(DynamicMemoryResource, BasicAllocation) {
    DynamicMemoryResource resource;
    
    void* ptr = resource.allocate(100, alignof(std::max_align_t));
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(resource.allocated_count(), 1);
    
    resource.deallocate(ptr, 100, alignof(std::max_align_t));
    EXPECT_EQ(resource.allocated_count(), 0);
}

TEST(DynamicMemoryResource, MultipleAllocations) {
    DynamicMemoryResource resource;
    
    void* ptr1 = resource.allocate(50, 8);
    void* ptr2 = resource.allocate(100, 16);
    void* ptr3 = resource.allocate(200, 32);
    
    EXPECT_EQ(resource.allocated_count(), 3);
    
    resource.deallocate(ptr2, 100, 16);
    EXPECT_EQ(resource.allocated_count(), 2);
    
    resource.deallocate(ptr1, 50, 8);
    resource.deallocate(ptr3, 200, 32);
    EXPECT_EQ(resource.allocated_count(), 0);
}

TEST(DynamicMemoryResource, AutoCleanup) {
    {
        DynamicMemoryResource resource;
        void* ptr1 = resource.allocate(100, 8);
        void* ptr2 = resource.allocate(200, 8);
        
        EXPECT_EQ(resource.allocated_count(), 2);
    }
    // Деструктор должен освободить всю память
    // Если нет segmentation fault - тест пройден
}

TEST(DynamicMemoryResource, DeallocateUnknownPointerThrows) {
    DynamicMemoryResource resource;
    void* fake_ptr = reinterpret_cast<void*>(0xDEADBEEF);
    
    EXPECT_THROW({
        resource.deallocate(fake_ptr, 100, 8);
    }, std::invalid_argument);
}

TEST(DynamicMemoryResource, IsEqual) {
    DynamicMemoryResource resource1;
    DynamicMemoryResource resource2;
    
    EXPECT_TRUE(resource1.is_equal(resource1));
    EXPECT_FALSE(resource1.is_equal(resource2));
}

TEST(DynamicMemoryResource, NullPointerDeallocation) {
    DynamicMemoryResource resource;
    // Не должно быть исключения
    resource.deallocate(nullptr, 0, 1);
    EXPECT_EQ(resource.allocated_count(), 0);
}

TEST(DynamicMemoryResource, DifferentAlignments) {
    DynamicMemoryResource resource;
    
    void* ptr1 = resource.allocate(10, 1);
    void* ptr2 = resource.allocate(10, 4);
    void* ptr3 = resource.allocate(10, 8);
    void* ptr4 = resource.allocate(10, 16);
    void* ptr5 = resource.allocate(10, 32);
    
    EXPECT_EQ(resource.allocated_count(), 5);
    
    resource.deallocate(ptr1, 10, 1);
    resource.deallocate(ptr2, 10, 4);
    resource.deallocate(ptr3, 10, 8);
    resource.deallocate(ptr4, 10, 16);
    resource.deallocate(ptr5, 10, 32);
}