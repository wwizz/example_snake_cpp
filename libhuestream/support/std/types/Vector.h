/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#ifndef STD_TYPES_VECTOR_H_
#define STD_TYPES_VECTOR_H_

#include <stddef.h>
#include <cstring>
#include <string>
#include <initializer_list>
#include <vector>
#include <utility>


namespace huesdk_lib {
    
    /** 
     Vector is a class template that stores elements of a given type in a linear arrangement and allows fast random access to any element.
     This is the simple version of std::vector.
     */
    template <class Type>
    class Vector {
    public:
        // Define iterator type
        typedef Type* iterator;

        // Constructor default
        Vector() : _data(nullptr), _capacity(0), _size(0) {
            reserve(_capacity_increment);  // reserve memory for some items
        }
        
        // Constructor with size
        explicit Vector(size_t n) : _data(nullptr), _capacity(0), _size(0) {
            resize(n);
        }
        
        // Destructor
        ~Vector() {
            clear();
        }
        
        // Constructor for an initializer_list of Type
        // cppcheck-suppress noExplicitConstructor
        Vector(const std::initializer_list<Type>& list) : _data(nullptr), _capacity(0), _size(0) {
            reserve(list.size());
            // push data
            for (typename std::initializer_list<Type>::const_iterator it = list.begin(); it != list.end(); ++it) {
                push_back(*it);
            }
        }
        
        // Constructor for an vector of Type
        explicit Vector(const std::vector<Type>& list) : _data(nullptr), _capacity(0), _size(0) {
            reserve(list.size());
            // push data
            for (typename std::vector<Type>::const_iterator it = list.begin(); it != list.end(); ++it) {
                push_back(*it);
            }
        }

        // Copy constructor
        Vector(const Vector& other) : _data(nullptr), _capacity(other._capacity), _size(other._size) {
            // allocate memory
            char* buffer = new char[_capacity * sizeof(Type)];
            _data = reinterpret_cast<Type*>(buffer);
            // copy data
            for (size_t i = 0; i < _size; ++i)
                new (_data + i) Type(*(other._data + i));
        }

        // Copy assignment operator
        Vector& operator=(const Vector& other) {
            if (this != &other) {
                clear();
                _capacity = other._capacity;
                _size = other._size;
                // allocate memory
                char* buffer = new char[_capacity * sizeof(Type)];
                _data = reinterpret_cast<Type*>(buffer);
                // copy data
                for (size_t i = 0; i < _size; ++i)
                    new (_data + i) Type(*(other._data + i));
            }
            return *this;
        }

        // Move constructor
        Vector(Vector&& other) {
            _data = other._data;
            _capacity = other._capacity;
            _size = other._size;
            other._data = nullptr;
            other._capacity = 0;
            other._size = 0;
        }

        // Move assignment operator
        Vector& operator=(Vector&& other) {
            if (this != &other) {
                clear();
                _data = other._data;
                _capacity = other._capacity;
                _size = other._size;
                other._data = nullptr;
                other._capacity = 0;
                other._size = 0;
            }
            return *this;
        }

        // Clear 
        void clear() {
            // destructor data
            for (size_t i = 0; i < _size; ++i)
                (_data + i)->~Type();
            // deallocate memory
            char* buffer = reinterpret_cast<char*>(_data);
            delete[] buffer;
            _data = nullptr;
            _capacity = 0;
            _size = 0;
        }

        // Return true if empty
        bool empty() const { 
            return _size == 0;
        }

        // Return size
        size_t size() const {
            return _size;
        }

        // Push back by copy
        void push_back(const Type& t) {
            if (_size == _capacity)
                reserve(_capacity + _capacity_increment);
            // copy constructor
            new(_data + _size) Type(t);
            ++_size;
        }

        // Push front by copy
        void push_front(const Type& t) {
            reallocate(_size + 1, 1);
            _capacity = _size + 1;
            // copy constructor
            new(_data) Type(t);
            ++_size;
        }

        // Push back by move
        void push_back(Type&& t) {
            if (_size == _capacity)
                reserve(_capacity + _capacity_increment);
            // move constructor
            new(_data + _size) Type(static_cast<Type&&>(t));
            ++_size;
        }

        // Insert a vector of elements
        void insert(const Vector<Type>& list) {
            for (Type& t : list) {
                push_back(t);
            }
        }
        
        // Push front by move
        void push_front(Type&& t) {
            reallocate(_size + 1, 1);
            _capacity = _size + 1;
            // move constructor
            new(_data) Type(static_cast<Type&&>(t));
            ++_size;
        }

        // Get item 
        Type& operator[](size_t i) {
            return *(_data + i);
        }

        // Get const item
        const Type& operator[](size_t i) const {
            return *(_data + i);
        }

        // begin iterator
        iterator begin() const { 
            return _data;
        }

        // end iterator
        iterator end() const { 
            return _data + _size;
        }

        // Erase by iterator
        iterator erase(iterator position) {
            if ( (position >= begin()) && (position < end()) ) {
                --_size;
                // destructor
                position->Type::~Type();
                // move elements
                for (iterator it = position; it < end(); it++) {
                    new (it) Type(std::move(*(it + 1)));
                    (it + 1)->Type::~Type();
                }
            }
            return position;
        }

        // Erase by index
        void erase(size_t index) {
            erase(begin() + index);
        }

        // Removes all elements that are equal to value
        void remove(const Type& value) {
            // use remove_if
            remove_if([value](const Type& item) { return item == value; });
        }

        // Removes all elements for which predicate p returns true
        template< class UnaryPredicate >
        void remove_if(UnaryPredicate p) {
            // go through items
            for (size_t i = 0; i < _size; ++i) {
                if ( p(_data[i]) ) {  // predicate p returns true
                    erase(i);
                    --i;
                }
            }
        }

        // Find iterator to the element for which predicate p returns true
        template< class UnaryPredicate >
        iterator find_if(UnaryPredicate p) {
            // go through items
            for (size_t i = 0; i < _size; ++i) {
                if ( p(_data[i]) ) {  // predicate p returns true
                    return _data + i;
                }
            }
            return end();
        }
        
        Type pop_front() {
            Type first_element = *(_data);
            // remove first element
            erase(static_cast<size_t>(0));
            
            return first_element;
        }
        
        // compare to the other Vector
        bool is_equal(const Vector& other) const {
            // compare size
            if (_size != other._size)
                return false;
            // compare items
            for (size_t i = 0; i < _size; ++i) {
                if (*(_data + i) != other[i])
                    return false;
            }
            return true;
        }
        
        // return the first element
        Type front() const {
            return *(_data);
        }

        // return the last element
        Type back() const {
            return *(_data+_size-1);
        }
        
    protected:
        // reserve memory
        void reserve(size_t n) {
            if (n > _capacity) {
                reallocate(n, 0);
                _capacity = n;
            }
        }

        // resize
        void resize(size_t n) {
            if (n < _size) {
                // destructor data
                for (size_t i = n; i < _size; ++i) {
                    (_data + i)->~Type();
                }
                _size = n;
            }
            if (n > _capacity)
                reserve(n);
            // constructor data 
            for (size_t i = _size; i < n; ++i) {
                new(_data + i) Type(); 
            }
            _size = n;
        }

        // reallocate data to the new buffer begining from the specified start position
        void reallocate(size_t new_size, size_t start_position) {
            size_t new_size_in_chars = new_size * sizeof(Type);
            Type* new_data = reinterpret_cast<Type*>(new char[new_size_in_chars]);

            if (_data != nullptr) {
                // ASSUME : new_size >= start_position + _size
                for (size_t i = 0; i < _size; i++) {
                    // copy elements to the new buffer
                    new (new_data + (start_position + i)) Type(std::move(_data[i]));
                    // deallocate element in the old buffer
                    (_data + i)->~Type();
                }

                delete[] reinterpret_cast<char*>(_data);
            }
            _data = new_data;
        }

        Type*  _data;        // array of pointers
        size_t _capacity;    // allocated
        size_t _size;        // used

        const size_t _capacity_increment = 5;  // increment of capacity
    };


    /**
    UniqueVectorPtr is a Vector that free memory of items in destructor. 
    It is moveable but not copyable like unique_ptr.
    */
    template <class Type>
    class UniqueVectorPtr : public Vector<Type> {
    public:
        // Define iterator type
        typedef Type* iterator;

        // Constructor default
        UniqueVectorPtr() : Vector<Type>() {
        }

        // Constructor from Vector
        explicit UniqueVectorPtr(const Vector<Type>& data) : Vector<Type>(data) {
        }

        // Constructor with size
        explicit UniqueVectorPtr(size_t n) : Vector<Type>(n) {
        }

        // Destructor
        ~UniqueVectorPtr() {
            clear();
        }

        // Move constructor
        UniqueVectorPtr(UniqueVectorPtr&& other) : Vector<Type>(std::move(other)), _delete_items(other._delete_items) {
            other._delete_items = false;
        }
        
        // No copy constructor
        UniqueVectorPtr(const UniqueVectorPtr& other) = delete;

        // No copy assignment
        UniqueVectorPtr& operator=(const UniqueVectorPtr&) = delete;

        // Move assignment
        UniqueVectorPtr& operator=(UniqueVectorPtr&& other) {
            if (this != &other) {
                clear();
                Vector<Type>::operator=(other);
                _delete_items = other._delete_items;
                other._delete_items = false;
            }
            return *this;
        }

        // Clear 
        void clear() {
            if (_delete_items) {
                for (size_t i = 0; i < Vector<Type>::_size; ++i)
                    delete *(Vector<Type>::_data + i);
            }
            Vector<Type>::clear();
        }

        // Convert to Vector 
        const Vector<Type>& to_vector() const {
            return *this;
        }

        // Return size
        size_t size() const {
            return Vector<Type>::size();
        }

        // Push back by copy
        void push_back(const Type& t) {
            Vector<Type>::push_back(t);
        }

        // Push back by move
        void push_back(Type&& t) {
            Vector<Type>::push_back(t);
        }

        // Get item 
        Type& operator[](size_t i) {
            return Vector<Type>::operator[](i);
        }

        // Get const item
        const Type& operator[](size_t i) const {
            return Vector<Type>::operator[](i);
        }

        // Removes all elements that are equal to value
        void remove(const Type& value) {
            // use remove_if
            remove_if([value](const Type& item) { return item == value; });
        }

        // Removes all elements for which predicate p returns true
        template< class UnaryPredicate >
        void remove_if(UnaryPredicate p) {
            // go through items
            for (size_t i = 0; i < Vector<Type>::_size; ++i) {
                if (p(Vector<Type>::_data[i])) {  // predicate p returns true
                    // delete data
                    if (_delete_items)
                        delete *(Vector<Type>::_data + i);
                    // erase from vector
                    Vector<Type>::erase(i);
                    --i;
                }
            }
        }

        void set_delete_items(bool delete_items) {
            _delete_items = delete_items;
        }

    private:
        bool _delete_items = true;
    };

}  // namespace huesdk_lib

#endif  // STD_TYPES_VECTOR_H_
