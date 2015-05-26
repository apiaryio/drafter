//
//  refract/ComparableVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef _REFRACT_COMPARABLEVISITOR_H_
#define _REFRACT_COMPARABLEVISITOR_H_

#include "Visitor.h"
#include "sos.h"
#include <string>

namespace refract
{

    // Forward declarations of Elements
    struct IElement;

    class ComparableVisitor : public IVisitor
    {
        std::string compare_to;
        bool result;

    public:

        ComparableVisitor(const std::string& str) : compare_to(str), result(false)
        {
        }

        template <typename T, typename U>
        bool IsEqual(const T& first, const U& second)
        {
            return false;
        }

        template <typename T>
        bool IsEqual(const T& first, const T& second)
        {
            return first == second;
        }

        template <typename E>
        void visit(const E& e)
        {
            result = IsEqual(compare_to, e.value);
        }

        virtual void visit(const IElement& e)
        {
            throw LogicError("Fallback impl - behavioration for Base class IElement is not defined");
        }

        operator bool() const
        {
            return result;
        }
    };


}; // namespace refract

#endif // #ifndef _REFRACT_COMPARABLEVISITOR_H_
