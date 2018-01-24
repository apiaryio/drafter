//
//  test/Tracker.h
//  test-librefract
//
//  Created by Thomas Jandecka on 27/08/2016
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include <vector>
#include <algorithm>

#ifndef TEST_REFRACT_TRACKER_INCLUDE
#define TEST_REFRACT_TRACKER_INCLUDE

template <typename ChildT>
class tracked
{
public:
    using objects_t = std::vector<ChildT*>;

private:
    static objects_t& objects()
    {
        static objects_t objects_;
        return objects_;
    }

public:
    static const ChildT& last_instance()
    {
        return *objects().back();
    }
    static objects_t& instances()
    {
        return objects();
    }

public:
    tracked()
    {
        objects().push_back(static_cast<ChildT*>(this));
    }

    tracked(const tracked&)
    {
        objects().push_back(static_cast<ChildT*>(this));
    }

    tracked(tracked&&)
    {
        objects().push_back(static_cast<ChildT*>(this));
    }

    tracked& operator=(const tracked&) = default;
    tracked& operator=(tracked&&) = default;

    ~tracked()
    {
        auto& objects_ = objects();
        objects_.erase(std::remove(objects_.begin(), objects_.end(), static_cast<ChildT*>(this)), objects_.end());
    }
};

#endif
