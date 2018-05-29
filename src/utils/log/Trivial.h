//
//  utils/log/trivial_log.h
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_LOG_TRIVIAL_H
#define DRAFTER_UTILS_LOG_TRIVIAL_H

#include <fstream>
#include <mutex>
#include <thread>

#define ENABLE_LOGGING (drafter::utils::log::trivial_log::instance().enable())

// clang-format off
#define LOG(svrty) (drafter::utils::log::trivial_entry<drafter::utils::log::svrty>{ drafter::utils::log::trivial_log::instance(), __LINE__, __FILE__ })
// clang-format on

namespace drafter
{
    namespace utils
    {
        namespace log
        {
            enum severity
            {
                debug,
                info,
                warning,
                error,
            };

            const char* severity_to_str(severity s);

            class trivial_log
            {
                mutable std::mutex write_mtx_;
                std::ofstream out_;
                bool enabled_ = false;

            public:
                static trivial_log& instance();

            private:
                trivial_log(const char* out_path);

            public:
                std::mutex& mtx() const
                {
                    return write_mtx_;
                }

                std::ostream& out()
                {
                    return out_;
                }

                void enable()
                {
                    std::lock_guard<std::mutex> lock(write_mtx_);
                    enabled_ = true;
                }

                bool enabled() const noexcept
                {
                    return enabled_;
                }
            };

            template <severity SEVERITY>
            class trivial_entry
            {
                trivial_log& log_;
                std::lock_guard<std::mutex> log_lock_;

            public:
                trivial_entry(trivial_log& log, size_t line, const char* file) : log_(log), log_lock_(log_.mtx())
                {
                    if (log.enabled()) {
                        log_.out() << '[' << severity_to_str(SEVERITY) << "]";
                        log_.out() << '[' << std::this_thread::get_id() << "]";
                        log_.out() << '[' << file << ':' << line << "] ";
                    }
                }

                trivial_entry(const trivial_entry&) = delete;
                trivial_entry(trivial_entry&&) = delete;

                trivial_entry& operator=(const trivial_entry&) = delete;
                trivial_entry& operator=(trivial_entry&&) = delete;

                ~trivial_entry()
                {
                    if (log_.enabled())
                        log_.out() << '\n'; // TODO @tjanc@ could throw
                }

                template <typename T>
                trivial_entry& operator<<(T&& obj)
                {
                    if (log_.enabled())
                        log_.out() << std::forward<T>(obj);
                    return *this;
                }
            };

#ifndef DEBUG
            template <>
            class trivial_entry<debug>
            {
            public:
                constexpr trivial_entry(trivial_log& log, size_t line, const char* file) noexcept {}

                template <typename T>
                const trivial_entry<debug>& operator<<(T&& obj) const noexcept
                {
                    return *this;
                }
            };
#endif
        }
    }
}

#endif
