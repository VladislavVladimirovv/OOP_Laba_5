#ifndef COMPLEX_TYPE_H
#define COMPLEX_TYPE_H

#include <iostream>
#include <string>
#include <utility>

struct ComplexType
{
    int id;
    std::string name;
    double value;

    ComplexType() : id(0), name(""), value(0.0)
    {
        std::cout << "ComplexType default constructed\n";
    }

    ComplexType(int i, std::string n, double v)
        : id(i), name(std::move(n)), value(v)
    {
        std::cout << "ComplexType constructed: " << id << ", " << name << ", " << value << "\n";
    }

    ComplexType(const ComplexType &other)
        : id(other.id), name(other.name), value(other.value)
    {
        std::cout << "ComplexType copy constructed: " << id << "\n";
    }

    ComplexType(ComplexType &&other) noexcept
        : id(std::exchange(other.id, 0)),
          name(std::move(other.name)),
          value(std::exchange(other.value, 0.0))
    {
        std::cout << "ComplexType move constructed: " << id << "\n";
    }

    ComplexType &operator=(const ComplexType &other)
    {
        if (this != &other)
        {
            id = other.id;
            name = other.name;
            value = other.value;
            std::cout << "ComplexType copy assigned: " << id << "\n";
        }
        return *this;
    }

    ComplexType &operator=(ComplexType &&other) noexcept
    {
        if (this != &other)
        {
            id = std::exchange(other.id, 0);
            name = std::move(other.name);
            value = std::exchange(other.value, 0.0);
            std::cout << "ComplexType move assigned: " << id << "\n";
        }
        return *this;
    }

    ~ComplexType()
    {
        std::cout << "ComplexType destroyed: " << id << "\n";
    }

    friend std::ostream &operator<<(std::ostream &os, const ComplexType &ct)
    {
        os << "{" << ct.id << ", \"" << ct.name << "\", " << ct.value << "}";
        return os;
    }
};

#endif