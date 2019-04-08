//
//  refract/Utils.h
//  librefract
//
//  Created by Thomas Jandecka on 07/11/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#include "Utils.h"
#include "Element.h"

#include <algorithm>

using namespace refract;

namespace
{
    struct Equal {
        const IElement& rhs;

        template <typename ElementT>
        bool operator()(const ElementT& lhs)
        {
            if (auto rhsptr = dynamic_cast<const ElementT*>(&rhs)) {
                return                                             //
                    (lhs.empty() == rhs.empty()) &&                //
                    (lhs.attributes() == rhs.attributes()) &&      //
                    (lhs.meta() == rhs.meta()) &&                  //
                    (lhs.empty() || (lhs.get() == rhsptr->get())); //
            } else
                return false;
        }
    };
}

bool refract::operator==(const IElement& lhs, const IElement& rhs) noexcept
{
    return visit(lhs, Equal{ rhs });
}

bool refract::operator!=(const IElement& lhs, const IElement& rhs) noexcept
{
    return !(lhs == rhs);
}

bool refract::operator==(const InfoElements& lhs, const InfoElements& rhs) noexcept
{
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(),
               lhs.end(),
               rhs.begin(),
               [](const InfoElements::value_type& l, const InfoElements::value_type& r) {
                   return (l.first == r.first) && (*l.second == *r.second);
               });
}

bool refract::operator!=(const InfoElements& lhs, const InfoElements& rhs) noexcept
{
    return !(lhs == rhs);
}
