#ifndef DIRTY_FLAG_HPP
#define DIRTY_FLAG_HPP

#include <memory>

namespace clean
{

template<typename T>
class dirty_flag
{
public:
    dirty_flag()
        : m_data(),
          m_dirty(true)
    { }

    dirty_flag(const T &data)
        : m_data(data),
          m_dirty(true)
    { }

    explicit dirty_flag(T &&data)
        : m_data(std::move(data)),
          m_dirty(true)
    { }

    dirty_flag<T> &operator =(const T &other)
    {
        m_data = other;
        m_dirty = true;
        return *this;
    }
    dirty_flag<T> &operator =(const dirty_flag<T> &other)
    {
        m_data = other.m_data;
        m_dirty = true;
        return *this;
    }

    bool is_dirty() const
    {
        return m_dirty;
    }

    void set_dirty()
    {
        m_dirty = true;
    }

    void clear_dirty()
    {
        m_dirty = false;
    }

    T& value()
    {
        return m_data;
    }

    T& operator*()
    {
        return value();
    }

private:
    T m_data;
    bool m_dirty;
};

}
#endif // DIRTY_FLAG_HPP
