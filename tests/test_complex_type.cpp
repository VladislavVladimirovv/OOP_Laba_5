#include <gtest/gtest.h>
#include "complex_type.h"
#include <sstream>

// Отключаем вывод в cout для тестов
class ComplexTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Сохраняем старый буфер
        old_buf = std::cout.rdbuf();
        // Перенаправляем cout в stringstream
        std::cout.rdbuf(ss.rdbuf());
    }
    
    void TearDown() override {
        // Восстанавливаем буфер
        std::cout.rdbuf(old_buf);
    }
    
    std::stringstream ss;
    std::streambuf* old_buf;
};

TEST_F(ComplexTypeTest, Constructors) {
    ComplexType ct1;
    EXPECT_EQ(ct1.id, 0);
    EXPECT_EQ(ct1.name, "");
    EXPECT_EQ(ct1.value, 0.0);
    
    ComplexType ct2(42, "Test", 3.14);
    EXPECT_EQ(ct2.id, 42);
    EXPECT_EQ(ct2.name, "Test");
    EXPECT_EQ(ct2.value, 3.14);
}

TEST_F(ComplexTypeTest, CopyConstructor) {
    ComplexType original(100, "Original", 99.9);
    ComplexType copy(original);
    
    EXPECT_EQ(copy.id, 100);
    EXPECT_EQ(copy.name, "Original");
    EXPECT_EQ(copy.value, 99.9);
    
    // Проверяем глубокое копирование
    copy.name = "Modified";
    EXPECT_EQ(original.name, "Original");
    EXPECT_EQ(copy.name, "Modified");
}

TEST_F(ComplexTypeTest, StreamOutput) {
    ComplexType ct(123, "Test Object", 45.67);
    
    std::stringstream ss;
    ss << ct;
    
    std::string expected = "{123, \"Test Object\", 45.67}";
    EXPECT_EQ(ss.str(), expected);
}

TEST_F(ComplexTypeTest, Assignment) {
    ComplexType ct1(1, "First", 1.1);
    ComplexType ct2(2, "Second", 2.2);
    
    ct2 = ct1;
    EXPECT_EQ(ct2.id, 1);
    EXPECT_EQ(ct2.name, "First");
    EXPECT_EQ(ct2.value, 1.1);
}

TEST_F(ComplexTypeTest, MoveConstructor) {
    ComplexType original(200, "ToMove", 50.5);
    ComplexType moved(std::move(original));
    
    EXPECT_EQ(moved.id, 200);
    EXPECT_EQ(moved.name, "ToMove");
    EXPECT_EQ(moved.value, 50.5);
    
    // Проверяем, что original был перемещен
    EXPECT_EQ(original.id, 0);
    EXPECT_EQ(original.name, "");
    EXPECT_EQ(original.value, 0.0);
}

// Дополнительные тесты можно добавить
TEST_F(ComplexTypeTest, MoveAssignment) {
    ComplexType ct1(10, "Source", 10.0);
    ComplexType ct2(20, "Destination", 20.0);
    
    ct2 = std::move(ct1);
    
    EXPECT_EQ(ct2.id, 10);
    EXPECT_EQ(ct2.name, "Source");
    EXPECT_EQ(ct2.value, 10.0);
    
    EXPECT_EQ(ct1.id, 0);
    EXPECT_EQ(ct1.name, "");
    EXPECT_EQ(ct1.value, 0.0);
}

TEST_F(ComplexTypeTest, SelfAssignment) {
    ComplexType ct(30, "Self", 30.0);
    
    ct = ct;  // Самоприсваивание должно работать корректно
    
    EXPECT_EQ(ct.id, 30);
    EXPECT_EQ(ct.name, "Self");
    EXPECT_EQ(ct.value, 30.0);
}