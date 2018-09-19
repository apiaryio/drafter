//
//  refract/JsonUtils.cc
//  librefract
//
//  Created by Thomas Jandecka on 16/08/2018
//  Copyright (c) 2018 Apiary Inc. All rights reserved.
//

#include "JsonUtils.h"

#include "../utils/log/Trivial.h"
#include "Element.h"
#include "dsd/Bool.h"
#include "dsd/Number.h"
#include "dsd/String.h"

using namespace drafter::utils::log;
using namespace drafter::utils;
using namespace refract;

so::String utils::instantiate(const dsd::String& e)
{
    return so::String{ e.get() };
}

so::Number utils::instantiate(const dsd::Number& e)
{
    return so::Number{ e.get() };
}

so::Value utils::instantiate(const dsd::Boolean& e)
{
    return e ? //
        so::Value{ in_place_type<so::True>{} } :
        so::Value{ in_place_type<so::False>{} };
}

so::String utils::instantiateEmpty(const StringElement& e)
{
    return so::String{};
}

so::Number utils::instantiateEmpty(const NumberElement& e)
{
    return so::Number{};
}

so::Value utils::instantiateEmpty(const BooleanElement& e)
{
    return so::False{};
}

const IElement& utils::resolve(const RefElement& element)
{
    const auto& resolvedEntry = element.attributes().find("resolved");
    if (resolvedEntry == element.attributes().end()) {
        LOG(error) << "expected all references to be resolved in backend";
        assert(false);
        return element;
    }

    assert(resolvedEntry->second);
    return *resolvedEntry->second;
}
