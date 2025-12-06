#ifndef DYNAMIC_LIST_H
#define DYNAMIC_LIST_H

#include <memory_resource>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <iostream>

template<typename T>
class DynamicList {
private:
    struct Node {
        T data;
        Node* next;
        
        template<typename... Args>
        explicit Node(Args&&... args) 
            : data(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;
    size_t size_;
    std::pmr::memory_resource* resource_;
    
public:
    class Iterator {
    private:
        Node* current;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        
        explicit Iterator(Node* node = nullptr) noexcept : current(node) {}
        Iterator(const Iterator&) noexcept = default;
        Iterator& operator=(const Iterator&) noexcept = default;
        
        Iterator& operator++() {
            if (current) {
                current = current->next;
            }
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        reference operator*() const {
            if (!current) {
                throw std::runtime_error("Dereferencing end iterator");
            }
            return current->data;
        }
        
        pointer operator->() const {
            if (!current) {
                throw std::runtime_error("Dereferencing end iterator");
            }
            return &current->data;
        }
        
        bool operator==(const Iterator& other) const noexcept {
            return current == other.current;
        }
        
        bool operator!=(const Iterator& other) const noexcept {
            return !(*this == other);
        }
    };
    
    explicit DynamicList(std::pmr::memory_resource* resource = nullptr)
        : head(nullptr), tail(nullptr), size_(0),
          resource_(resource ? resource : std::pmr::get_default_resource()) {}
    
    DynamicList(std::initializer_list<T> init,
                std::pmr::memory_resource* resource = nullptr)
        : head(nullptr), tail(nullptr), size_(0),
          resource_(resource ? resource : std::pmr::get_default_resource()) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    DynamicList(const DynamicList& other)
        : head(nullptr), tail(nullptr), size_(0),
          resource_(other.resource_) {
        copy_from(other);
    }
    
    DynamicList(DynamicList&& other) noexcept
        : head(other.head), tail(other.tail), size_(other.size_),
          resource_(other.resource_) {
        other.head = nullptr;
        other.tail = nullptr;
        other.size_ = 0;
        other.resource_ = std::pmr::get_default_resource();
    }
    
    ~DynamicList() {
        clear();
    }
    
    DynamicList& operator=(const DynamicList& other) {
        if (this != &other) {
            clear();
            resource_ = other.resource_;
            copy_from(other);
        }
        return *this;
    }
    
    DynamicList& operator=(DynamicList&& other) noexcept {
        if (this != &other) {
            clear();
            
            head = other.head;
            tail = other.tail;
            size_ = other.size_;
            resource_ = other.resource_;
            
            other.head = nullptr;
            other.tail = nullptr;
            other.size_ = 0;
            other.resource_ = std::pmr::get_default_resource();
        }
        return *this;
    }
    
    T& front() {
        if (!head) throw std::out_of_range("List is empty");
        return head->data;
    }
    
    const T& front() const {
        if (!head) throw std::out_of_range("List is empty");
        return head->data;
    }
    
    T& back() {
        if (!tail) throw std::out_of_range("List is empty");
        return tail->data;
    }
    
    const T& back() const {
        if (!tail) throw std::out_of_range("List is empty");
        return tail->data;
    }
    
    size_t size() const noexcept {
        return size_;
    }
    
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    void push_back(const T& value) {
        Node* new_node = create_node(value);
        add_node(new_node);
    }
    
    void push_back(T&& value) {
        Node* new_node = create_node(std::move(value));
        add_node(new_node);
    }
    
    template<typename... Args>
    void emplace_back(Args&&... args) {
        Node* new_node = create_node(std::forward<Args>(args)...);
        add_node(new_node);
    }
    
    void pop_front() {
        if (!head) return;
        
        Node* to_delete = head;
        head = head->next;
        
        destroy_node(to_delete);
        
        if (!head) tail = nullptr;
        size_--;
    }
    
    void clear() noexcept {
        while (head) {
            pop_front();
        }
    }
    
    Iterator begin() noexcept {
        return Iterator(head);
    }
    
    Iterator end() noexcept {
        return Iterator(nullptr);
    }
    
    std::pmr::memory_resource* get_allocator_resource() const noexcept {
        return resource_;
    }
    
    void swap(DynamicList& other) noexcept {
        using std::swap;
        swap(head, other.head);
        swap(tail, other.tail);
        swap(size_, other.size_);
        swap(resource_, other.resource_);
    }
    
private:
    Node* create_node(const T& value) {
        std::pmr::polymorphic_allocator<Node> alloc(resource_);
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, value);
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
        return new_node;
    }
    
    Node* create_node(T&& value) {
        std::pmr::polymorphic_allocator<Node> alloc(resource_);
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, std::move(value));
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
        return new_node;
    }
    
    template<typename... Args>
    Node* create_node(Args&&... args) {
        std::pmr::polymorphic_allocator<Node> alloc(resource_);
        Node* new_node = alloc.allocate(1);
        try {
            alloc.construct(new_node, std::forward<Args>(args)...);
        } catch (...) {
            alloc.deallocate(new_node, 1);
            throw;
        }
        return new_node;
    }
    
    void add_node(Node* node) {
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size_++;
    }
    
    void destroy_node(Node* node) noexcept {
        if (node) {
            std::pmr::polymorphic_allocator<Node> alloc(resource_);
            alloc.destroy(node);
            alloc.deallocate(node, 1);
        }
    }
    
    void copy_from(const DynamicList& other) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }
};

template<typename T>
void swap(DynamicList<T>& lhs, DynamicList<T>& rhs) noexcept {
    lhs.swap(rhs);
}

#endif 