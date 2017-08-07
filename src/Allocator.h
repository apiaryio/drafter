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
        /// Block of untyped memory
        ///
        struct Block {
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
            Block allocate(size_t size)
            {
                Block r = Primary::allocate(size);
                if (!r.ptr) r = Fallback::allocate(size);
                return r;
            }

            void deallocate(Block b)
            {
                if (Primary::owns(b))
                    Primary::deallocate(b);
                else
                    Fallback::deallocate(b);
            }

            bool owns(Block b) const
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
            Block allocate(size_t size)
            {
                return Block{std::malloc(size), size};
            }

            void deallocate(Block b) { std::free(b.ptr); }
        };

        ///
        /// Always allocates a block with nullptr and no size; asserts when
        /// deallocate is called by non-nullptr block.
        ///
        class NullAllocator
        {
           public:
            Block allocate(size_t) { return Block{nullptr, 0}; }
            void deallocate(Block b) { assert(!b.ptr); }
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
            Block allocate(size_t n) noexcept
            {
                auto n1 = round_aligned(n);
                if (n1 > (b_ + Size) - p_) return Block{nullptr, 0};
                Block result{p_, n};
                p_ += n1;
                return result;
            }

            void deallocate(Block b) noexcept
            {
                if (b.ptr + round_aligned(b.size) == p_) p_ = b.ptr;
            }

            bool owns(Block b) const noexcept
            {
                return b.ptr >= b_ && b.ptr < b_ + Size;
            }

            void deallocateAll() noexcept { p_ = b_; }
        };

        template <typename Allocator>
        struct deleter {
            Allocator& alloc;
            size_t size;

            template <typename T>
            void operator()(T* ptr)
            {
                alloc.deallocate(memory::Block{ptr, size});
            }
        };
    }

    template <typename T, typename Allocator, typename... Args>
    auto make_unique(Allocator& alloc, Args&&... args)
    {
        using namespace memory;

        Block blk = alloc.allocate(sizeof(T));
        assert(blk.ptr);

        return std::unique_ptr<T, memory::deleter<Allocator>>{
            new (blk.ptr) T(std::forward<Args>(args)...), {alloc, blk.size}};
    }

    using DefaultAllocator =
        memory::FallbackAllocator<memory::StackAllocator<16384>,
                                  memory::Mallocator>;
}

#endif
