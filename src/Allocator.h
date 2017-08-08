#ifndef REFRACT_ALLOCATOR_INCLUDE_
#define REFRACT_ALLOCATOR_INCLUDE_

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <type_traits>

namespace refract
{
    namespace memory
    {
        ///
        /// Untyped block of memory
        ///
        struct block_t {
            void* ptr;
            size_t size;
        };

        ///
        /// Allocator using Fallback allocator if allocating through Primary
        /// allocator proves unsuccessful
        ///
        template <typename Primary, typename Fallback>
        class FallbackAllocator : private Primary, private Fallback
        {
           public:
            block_t allocate(size_t size)
            {
                block_t r = Primary::allocate(size);
                if (!r.ptr) r = Fallback::allocate(size);
                return r;
            }

            void deallocate(block_t b)
            {
                if (Primary::owns(b))
                    Primary::deallocate(b);
                else
                    Fallback::deallocate(b);
            }

            bool owns(block_t b) const
            {
                return Primary::owns(b) || Fallback::owns(b);
            }
        };

        ///
        /// Plain old malloc; owns not implemented.
        ///
        class Mallocator
        {
           public:
            block_t allocate(size_t size)
            {
                return block_t{std::malloc(size), size};
            }

            void deallocate(block_t b) { std::free(b.ptr); }
        };

        ///
        /// Always allocates a block_t with nullptr and no size; asserts when
        /// deallocate is called by non-nullptr block_t.
        ///
        class NullAllocator
        {
           public:
            block_t allocate(size_t) { return block_t{nullptr, 0}; }
            void deallocate(block_t b) { assert(!b.ptr); }
        };

        ///
        /// Allocator reserving Size chars on stack; deallocation limited to
        /// last allocated element
        ///
        template <size_t Size, size_t Addressing = alignof(std::max_align_t)>
        class StackAllocator
        {
            char b_[Size];
            char* p_;

            static size_t round_aligned(size_t n) noexcept
            {
                auto n1 = (n / Addressing) * Addressing;
                if (n1 < n) n1 += Addressing;
                return n1;
            }

           public:
            StackAllocator() : p_(b_) {}

           public:
            block_t allocate(size_t n) noexcept
            {
                auto n1 = round_aligned(n);
                if (n1 > (b_ + Size) - p_) return block_t{nullptr, 0};
                block_t result{p_, n};
                p_ += n1;
                return result;
            }

            void deallocate(block_t b) noexcept
            {
                if (b.ptr + round_aligned(b.size) == p_) p_ = b.ptr;
            }

            bool owns(block_t b) const noexcept
            {
                return b.ptr >= b_ && b.ptr < b_ + Size;
            }

            void deallocateAll() noexcept { p_ = b_; }
        };

        ///
        /// Functor calling deallocate with a block of captured size on captured
        /// allocator
        ///
        template <typename Allocator>
        struct deleter {
            Allocator& alloc;  /// allocator to be deallocated from
            size_t size;       /// amount to be deallocated

            ///
            /// Deallocate captured amount of bytes beginning at given address,
            /// using captured allocator
            ///
            /// @param ptr address to be deallocated from
            ///
            template <typename T>
            void operator()(T* ptr)
            {
                alloc.deallocate(block_t{ptr, size});
            }
        };

        ///
        /// Create an instance of T in memory provided by allocator alloc
        ///
        /// @params args    arguments to T's constructor
        ///
        /// @return unique_ptr calling deallocate on provided allocator upon
        /// destruction
        ///
        template <typename T, typename Allocator, typename... TArgs>
        auto make_unique_alloc(Allocator& alloc, TArgs&&... args)
        {
            auto blk = alloc.allocate(sizeof(T));
            assert(blk.ptr);

            return std::unique_ptr<T, memory::deleter<Allocator>>{
                new (blk.ptr) T(std::forward<TArgs>(args)...),
                {alloc, blk.size}};
        }
    }

    using DefaultAllocator =
        memory::FallbackAllocator<memory::StackAllocator<16384>,
                                  memory::Mallocator>;
}

#endif
