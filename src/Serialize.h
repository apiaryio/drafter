//
//  Serialize.h
//  drafter
//
//  Created by Zdenek Nemec on 5/3/13.
//  Copyright (c) 2013 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_SERIALIZE_H
#define DRAFTER_SERIALIZE_H

#include <string>
#include "BlueprintSourcemap.h"

#include "NodeInfo.h"

#include "refract/Element.h"
#include "refract/Registry.h"

#include "SerializeKey.h"

/** Version of API Blueprint serialization */
#define AST_SERIALIZATION_VERSION "4.0"
#define PARSE_RESULT_SERIALIZATION_VERSION "2.2"

// Forward declaration
namespace snowcrash
{

    template <typename T>
    struct ParseResult;
}

namespace drafter
{
    enum SerializeFormat
    {
        JSONFormat = 0, // JSON Format
        YAMLFormat,     // YAML Format
        UnknownFormat = -1
    };

    template <typename T>
    std::pair<bool, T> LiteralTo(const mson::Literal&);

    template <>
    std::pair<bool, refract::dsd::Boolean> LiteralTo<refract::dsd::Boolean>(const mson::Literal& literal);
    template <>
    std::pair<bool, refract::dsd::Number> LiteralTo<refract::dsd::Number>(const mson::Literal& literal);
    template <>
    std::pair<bool, refract::dsd::String> LiteralTo<refract::dsd::String>(const mson::Literal& literal);
}

#endif
