#ifndef REFRACT_MEMORY_INCLUDE_
#define REFRACT_MEMORY_INCLUDE_

#include <cassert>
#include <memory>

namespace refract
{
    template <typename T>
    using unique_ptr = std::unique_ptr<T>;

    template <typename T>
    using shared_ptr = std::shared_ptr<T>;

    template <typename T, typename... TArgs>
    auto make_unique(TArgs&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
    }

    template <typename T, typename... TArgs>
    auto make_shared(TArgs&&... args)
    {
        return std::make_shared<T>(std::forward<TArgs>(args)...);
    }
}

#endif
