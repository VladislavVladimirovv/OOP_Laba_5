#include "../include/dynamic_memory_resource.h"
#include "../include/dynamic_list.h"
#include "../include/complex_type.h"
#include <iostream>
#include <algorithm>

void demonstrate_int_list()
{
    std::cout << "\n=== Пример работы динамического массива с элементами типа int ===" << std::endl;

    DynamicMemoryResource resource;
    DynamicList<int> numbers(&resource);

    for (int i = 1; i <= 5; ++i)
    {
        numbers.push_back(i * 10);
    }

    std::cout << "Размер динамического массива: " << numbers.size() << std::endl;
    std::cout << "Количество выделенных блоков " << resource.allocated_count() << std::endl;

    std::cout << "Использование итераторов: ";
    for (auto it = numbers.begin(); it != numbers.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;

    std::cout << "Использование range-based for: ";
    for (const auto &num : numbers)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    auto found = std::find(numbers.begin(), numbers.end(), 30);
    if (found != numbers.end())
    {
        std::cout << "Найдено значение 30 в списке" << std::endl;
    }
}

void demonstrate_complex_type_list()
{
    std::cout << "\n=== Демонстрация работы Динамического массива со сложным типом ComplexType ===" << std::endl;

    DynamicMemoryResource resource;
    DynamicList<ComplexType> objects(&resource);

    std::cout << "\nДобавление объектов:" << std::endl;
    objects.emplace_back(1, "Объект 1", 1.1);
    objects.emplace_back(2, "Объект 2", 2.2);
    objects.emplace_back(3, "Объект 3", 3.3);

    std::cout << "\nСодержимое динамического массива:" << std::endl;
    for (const auto &obj : objects)
    {
        std::cout << "  " << obj << std::endl;
    }

    std::cout << "\nХарактеристика динамического массива:" << std::endl;
    std::cout << "Количество выделенных блоков: " << resource.allocated_count() << std::endl;
}

void demonstrate_memory_reuse()
{
    std::cout << "\n=== Демонстрация переиспользования памяти ===" << std::endl;

    DynamicMemoryResource resource;

    std::cout << "Первоначально выделенные блоки: " << resource.allocated_count() << std::endl;

    {
        DynamicList<int> temp_list(&resource);
        temp_list.push_back(100);
        temp_list.push_back(200);
        temp_list.push_back(300);

        std::cout << "После добавления 3-х элементов: " << resource.allocated_count() << " blocks" << std::endl;

        temp_list.pop_front();
        std::cout << "После pop_front: " << resource.allocated_count() << " blocks" << std::endl;
    }

    std::cout << "После уничтожения списка: " << resource.allocated_count() << " blocks" << std::endl;
}

void demonstrate_initializer_list()
{
    std::cout << "\n=== Демонстрация инициализатора списков ===" << std::endl;

    DynamicMemoryResource resource;

    DynamicList<int> list = {1, 2, 3, 4, 5};

    std::cout << "Список полученный при инициализации: ";
    for (const auto &item : list)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

int main()
{
    std::cout << "=== Демонстрация динамического массива с динамическим выделением памяти ===" << std::endl;

    try
    {
        demonstrate_int_list();
        demonstrate_complex_type_list();
        demonstrate_memory_reuse();
        demonstrate_initializer_list();
    }
    catch (const std::exception &e)
    {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}