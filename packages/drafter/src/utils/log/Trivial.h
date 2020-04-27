//
//  utils/log/trivial_log.h
//  librefract
//
//  Created by Thomas Jandecka on 16/02/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_UTILS_LOG_TRIVIAL_H
#define DRAFTER_UTILS_LOG_TRIVIAL_H

#include <mutex>
#include <thread>
#include <ostream>

#define ENABLE_LOGGING (drafter::utils::log::trivial_log::instance().enable())

// clang-format off
#define LOG(svrty) (drafter::utils::log::trivial_entry{ drafter::utils::log::trivial_log::instance(), drafter::utils::log::svrty, __LINE__, __FILE__ })
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

            class trivial_log;

            class trivial_entry
            {
                trivial_log& log_;
                severity severity_;
                std::lock_guard<std::mutex> log_lock_;

            public:
                trivial_entry(trivial_log& log, severity svrty, size_t line, const char* file);

                trivial_entry(const trivial_entry&) = delete;
                trivial_entry(trivial_entry&&) = delete;

                trivial_entry& operator=(const trivial_entry&) = delete;
                trivial_entry& operator=(trivial_entry&&) = delete;

                template <typename T>
                trivial_entry& operator<<(T&& obj);

                ~trivial_entry();
            };

            class trivial_log
            {
                mutable std::mutex write_mtx_;
                std::ostream* out_ = nullptr;

            public:
                static trivial_log& instance();

            private:
                trivial_log() = default;

            public:
                std::mutex& mtx() const;
                void enable();
                std::ostream* out();
            };

            template <typename T>
            trivial_entry& trivial_entry::operator<<(T&& obj)
            {
                if (auto* out = log_.out())
                    *out << std::forward<T>(obj);
                return *this;
            }
        } // namespace log
    }     // namespace utils
} // namespace drafter

#endif
