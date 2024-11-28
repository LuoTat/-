#pragma once

#include "openHL/core/hldef.h"

#include <string>
#include <memory>         // std::shared_ptr
#include <type_traits>    // std::enable_if

namespace hl
{

using std::nullptr_t;

template <typename Y> struct DefaultDeleter;

namespace sfinae
{
template <typename C, typename Ret, typename... Args>
struct has_parenthesis_operator
{
private:
    template <typename T>
    static constexpr std::true_type has_parenthesis_operator_check(typename std::is_same<typename std::decay<decltype(std::declval<T>().operator()(std::declval<Args>()...))>::type, Ret>::type*);

    template <typename> static constexpr std::false_type has_parenthesis_operator_check(...);

    typedef decltype(has_parenthesis_operator_check<C>(0)) type;

public:
    static constexpr bool value = type::value;
};
}    // namespace sfinae

template <typename T, typename = void>
struct has_custom_delete: public std::false_type
{};

template <typename T>
struct has_custom_delete<T, typename std::enable_if<sfinae::has_parenthesis_operator<DefaultDeleter<T>, void, T*>::value>::type>: public std::true_type
{};

template <typename T>
struct Ptr: public std::shared_ptr<T>
{
    inline Ptr() noexcept:
        std::shared_ptr<T>() {}

    inline Ptr(nullptr_t) noexcept:
        std::shared_ptr<T>(nullptr) {}

    template <typename Y, typename D> inline Ptr(Y* p, D d):
        std::shared_ptr<T>(p, d) {}

    template <typename D> inline Ptr(nullptr_t, D d):
        std::shared_ptr<T>(nullptr, d) {}

    template <typename Y> inline Ptr(const Ptr<Y>& r, T* ptr) noexcept:
        std::shared_ptr<T>(r, ptr) {}

    inline Ptr(const Ptr<T>& o) noexcept:
        std::shared_ptr<T>(o) {}

    inline Ptr(Ptr<T>&& o) noexcept:
        std::shared_ptr<T>(std::move(o)) {}

    template <typename Y> inline Ptr(const Ptr<Y>& o) noexcept:
        std::shared_ptr<T>(o) {}

    template <typename Y> inline Ptr(Ptr<Y>&& o) noexcept:
        std::shared_ptr<T>(std::move(o)) {}

    inline Ptr(const std::shared_ptr<T>& o) noexcept:
        std::shared_ptr<T>(o) {}

    inline Ptr(std::shared_ptr<T>&& o) noexcept:
        std::shared_ptr<T>(std::move(o)) {}

    template <typename Y>
    inline Ptr(const std::true_type&, Y* ptr):
        std::shared_ptr<T>(ptr, DefaultDeleter<Y>())
    {}

    template <typename Y>
    inline Ptr(const std::false_type&, Y* ptr):
        std::shared_ptr<T>(ptr)
    {}

    template <typename Y = T>
    inline Ptr(Y* ptr):
        Ptr(has_custom_delete<Y>(), ptr)
    {}

    template <typename Y>
    inline void reset(const std::true_type&, Y* ptr)
    {
        std::shared_ptr<T>::reset(ptr, DefaultDeleter<Y>());
    }

    template <typename Y>
    inline void reset(const std::false_type&, Y* ptr)
    {
        std::shared_ptr<T>::reset(ptr);
    }

    template <typename Y>
    inline void reset(Y* ptr)
    {
        Ptr<T>::reset(has_custom_delete<Y>(), ptr);
    }

    template <class Y, class Deleter>
    void reset(Y* ptr, Deleter d)
    {
        std::shared_ptr<T>::reset(ptr, d);
    }

    void reset() noexcept { std::shared_ptr<T>::reset(); }

    Ptr& operator=(const Ptr& o)
    {
        std::shared_ptr<T>::operator=(o);
        return *this;
    }

    template <typename Y> inline Ptr& operator=(const Ptr<Y>& o)
    {
        std::shared_ptr<T>::operator=(o);
        return *this;
    }

    T* operator->() const noexcept { return std::shared_ptr<T>::get(); }

    typename std::add_lvalue_reference<T>::type operator*() const noexcept { return *std::shared_ptr<T>::get(); }

    inline void release() { std::shared_ptr<T>::reset(); }

    inline operator T*() const { return std::shared_ptr<T>::get(); }

    inline bool empty() const { return std::shared_ptr<T>::get() == nullptr; }

    template <typename Y> inline Ptr<Y> staticCast() const noexcept { return std::static_pointer_cast<Y>(*this); }

    template <typename Y> inline Ptr<Y> constCast() const noexcept { return std::const_pointer_cast<Y>(*this); }

    template <typename Y> inline Ptr<Y> dynamicCast() const noexcept { return std::dynamic_pointer_cast<Y>(*this); }
};

template <typename _Tp, typename... A1> inline static Ptr<_Tp> makePtr(const A1&... a1)
{
    static_assert(!has_custom_delete<_Tp>::value, "Can't use this makePtr with custom DefaultDeleter");
    return (Ptr<_Tp>)std::make_shared<_Tp>(a1...);
}
}    // namespace hl