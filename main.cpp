#include <iostream>

template<typename T>
class Storage
{
public:
    Storage()=default;
    explicit Storage(size_t size_memory) :  size_{size_memory},
                                            data_{static_cast<T*>(::operator new(size_memory * sizeof(T)))}
    {}
    Storage(size_t size, T elem)
    {
        data_ = static_cast<T*>(::operator new(size * sizeof(T)));
        for(;size_ < size; ++size_)
        {
            new(data_+size_) T{elem};
        }
        used_ = size_;
    }
    Storage(Storage&& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(used_, other.used_);
        std::swap(data_, other.data_);
    }
    Storage& operator=(Storage&& other) noexcept
    {
        if(this != &other)
        {
            std::swap(size_, other.size_);
            std::swap(used_, other.used_);
            std::swap(data_, other.data_);
        }
        return *this;
    }
    Storage(Storage const& other)
    {
        data_ = static_cast<T*>(::operator new(other.size_ * sizeof(T)));
        for(; size_ < other.size_; ++size_)
        {
            new(data_+size_) T{other.data_[size_]};
        }

        size_ = other.size_;
        used_ = other.used_;
    }
    Storage& operator=(Storage const& other)
    {
        Storage s{other};

        std::swap(this, s);
    }
    ~Storage()
    {
        for(int i = 0; i < used_; ++i)
        {
            data_[i].~T();
        }
        ::operator delete(data_);
    }
protected:
    T* data_{};
    size_t size_{};
    size_t used_{};
};

template<typename T>
class vector : public Storage<T>
{
    using Storage<T>::size_;
    using Storage<T>::used_;
    using Storage<T>::data_;
public:
    vector()=default;
    ~vector()=default;

    explicit vector(std::size_t size) : Storage<T>{size}
    {}
    vector(std::size_t size, T elem) : Storage<T>{size, elem}
    {}
    vector(vector const& other) : Storage<T>{other.storage_}
    {}
    vector& operator=(vector const& other)
    {
        vector<T> s{other};

        std::swap(this, s);
    }
    vector(vector&& other) noexcept : Storage<T>{other}
    {}
    vector& operator=(vector&& other) noexcept
    {
        std::swap(*this, other);
    }

    template<typename Arg>
    void push_back(Arg&& elem)
    {
        if(size_ == used_)
            realloc(size_ * 2);

        new(data_ + used_) T{std::forward<Arg>(elem)};
        ++used_;
    }
    T &operator[](size_t id) noexcept { return data_[id]; }
    void swap(vector& v1, vector& v2)
    {
        std::swap(v1.size_, v2.size_);
        std::swap(v1.used_, v2.used_);
        std::swap(v1.data_, v2.data_);
    }
private:
    void realloc(size_t size)
    {
        vector v{size};

        for(int i = 0; i < used_; ++i)
        {
            new(v.data_+i) T{data_[i]};
        }
        v.used_ = used_;

        swap(*this, v);
    }
};

int main() {
    vector<int> v(2);
    for(int i = 0; i < 50; ++i)
        v.push_back(i);
    for(int i = 0; i < 55; ++i)
        std::cout << v[i] << " ";
    return 0;
}
