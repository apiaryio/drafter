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
    using objects_type = std::vector<ChildT*>;
    struct record_type {
        int default_constructor = 0;
        int copy_constructor = 0;
        int move_constructor = 0;
        int copy_assignment = 0;
        int move_assignment = 0;
        int destructor = 0;
    };

private:
    static objects_type& objects_()
    {
        static objects_type objects{};
        return objects;
    }

    static record_type& record_()
    {
        static record_type record{};
        return record;
    }

public:
    static const ChildT& last_instance()
    {
        return *objects_().back();
    }
    static const objects_type& instances()
    {
        return objects_();
    }

    static const record_type& record()
    {
        return record_();
    }

    static void reset_record()
    {
        record_() = record_type{};
    }

public:
    tracked()
    {
        ++record_().default_constructor;
        objects_().push_back(static_cast<ChildT*>(this));
    }

    tracked(const tracked&)
    {
        ++record_().copy_constructor;
        objects_().push_back(static_cast<ChildT*>(this));
    }

    tracked(tracked&&)
    {
        ++record_().move_constructor;
        objects_().push_back(static_cast<ChildT*>(this));
    }

    tracked& operator=(const tracked&)
    {
        ++record_().copy_assignment;
        return *this;
    }

    tracked& operator=(tracked&&)
    {
        ++record_().move_assignment;
        return *this;
    }

    ~tracked()
    {
        ++record_().destructor;
        auto& objects = objects_();
        objects.erase(std::remove(objects.begin(), objects.end(), static_cast<ChildT*>(this)), objects.end());
    }
};

#endif
