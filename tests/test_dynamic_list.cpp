#include <gtest/gtest.h>
#include "dynamic_list.h"
#include "dynamic_memory_resource.h"
#include "complex_type.h"
#include <algorithm>
#include <string>

TEST(DynamicList, EmptyList) {
    DynamicList<int> list;
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.begin(), list.end());
}

TEST(DynamicList, PushBack) {
    DynamicMemoryResource resource;
    DynamicList<int> list(&resource);
    
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
    EXPECT_EQ(resource.allocated_count(), 3);
}

TEST(DynamicList, FrontAndBack) {
    DynamicList<int> list;
    
    list.push_back(100);
    list.push_back(200);
    list.push_back(300);
    
    EXPECT_EQ(list.front(), 100);
    EXPECT_EQ(list.back(), 300);
}

TEST(DynamicList, EmptyListExceptions) {
    DynamicList<int> empty_list;
    
    EXPECT_THROW(empty_list.front(), std::out_of_range);
    EXPECT_THROW(empty_list.back(), std::out_of_range);
}

TEST(DynamicList, Iterators) {
    DynamicList<int> list;
    
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    int sum = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
    
    // Range-based for
    sum = 0;
    for (int value : list) {
        sum += value;
    }
    EXPECT_EQ(sum, 6);
}

TEST(DynamicList, CopyConstructor) {
    DynamicList<int> list;
    
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    
    DynamicList<int> copy(list);
    
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.front(), 10);
    EXPECT_EQ(copy.back(), 30);
    
    // Проверяем глубокое копирование
    list.pop_front();
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(copy.size(), 3);  // Копия не должна измениться
}

TEST(DynamicList, MoveConstructor) {
    DynamicMemoryResource resource;
    DynamicList<int> list(&resource);
    
    list.push_back(100);
    list.push_back(200);
    
    size_t original_blocks = resource.allocated_count();
    
    DynamicList<int> moved(std::move(list));
    
    EXPECT_EQ(moved.size(), 2);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(resource.allocated_count(), original_blocks);
}

TEST(DynamicList, AssignmentOperator) {
    DynamicList<int> list;
    
    list.push_back(5);
    list.push_back(15);
    
    DynamicList<int> another_list;
    another_list = list;
    
    EXPECT_EQ(another_list.size(), 2);
    EXPECT_EQ(another_list.front(), 5);
    EXPECT_EQ(another_list.back(), 15);
}

TEST(DynamicList, PopFront) {
    DynamicMemoryResource resource;
    DynamicList<int> list(&resource);
    
    list.push_back(100);
    list.push_back(200);
    list.push_back(300);
    
    EXPECT_EQ(resource.allocated_count(), 3);
    
    list.pop_front();
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 200);
    EXPECT_EQ(resource.allocated_count(), 2);
    
    list.pop_front();
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 300);
    
    list.pop_front();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(resource.allocated_count(), 0);
}

TEST(DynamicList, Clear) {
    DynamicMemoryResource resource;
    DynamicList<int> list(&resource);
    
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    
    EXPECT_EQ(resource.allocated_count(), 3);
    
    list.clear();
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(resource.allocated_count(), 0);
}

TEST(DynamicList, InitializerList) {
    DynamicList<int> list = {1, 2, 3, 4, 5};
    
    EXPECT_EQ(list.size(), 5);
    
    int expected = 1;
    for (int value : list) {
        EXPECT_EQ(value, expected++);
    }
}

TEST(DynamicList, EmplaceBack) {
    DynamicMemoryResource resource;
    DynamicList<std::string> string_list(&resource);
    
    string_list.emplace_back("Hello");
    string_list.emplace_back("World");
    string_list.emplace_back("!");
    
    EXPECT_EQ(string_list.size(), 3);
    EXPECT_EQ(string_list.front(), "Hello");
    EXPECT_EQ(string_list.back(), "!");
}

TEST(DynamicList, STLAlgorithms) {
    DynamicList<int> list;
    
    list.push_back(5);
    list.push_back(2);
    list.push_back(8);
    list.push_back(1);
    list.push_back(9);
    
    // std::find
    auto it = std::find(list.begin(), list.end(), 8);
    ASSERT_NE(it, list.end());
    EXPECT_EQ(*it, 8);
    
    // std::count
    int count = std::count(list.begin(), list.end(), 5);
    EXPECT_EQ(count, 1);
    
    // std::max_element
    auto max_it = std::max_element(list.begin(), list.end());
    EXPECT_EQ(*max_it, 9);
}

TEST(DynamicList, ComplexTypeOperations) {
    DynamicMemoryResource resource;
    DynamicList<ComplexType> objects(&resource);
    
    objects.emplace_back(1, "First", 10.5);
    objects.emplace_back(2, "Second", 20.5);
    
    EXPECT_EQ(objects.size(), 2);
    EXPECT_EQ(objects.front().id, 1);
    EXPECT_EQ(objects.back().id, 2);
    EXPECT_EQ(resource.allocated_count(), 2);
    
    // Копирование
    DynamicList<ComplexType> copy = objects;
    EXPECT_EQ(copy.size(), 2);
    EXPECT_EQ(copy.front().name, "First");
}


