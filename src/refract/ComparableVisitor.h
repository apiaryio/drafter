//
//  refract/ComparableVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 21/05/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_COMPARABLEVISITOR_H
#define REFRACT_COMPARABLEVISITOR_H

#include "Visitor.h"

namespace refract
{

    // Forward declarations of Elements
    struct IElement;
    struct MemberElement;

    class ComparableVisitor : public IVisitor
    {
        std::string compare_to;
        bool result;
        int compare;

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

    public:

        enum {
            key,
            value
        };

        ComparableVisitor(const std::string& str, const int compare = value);

        template <typename E>
        void visit(const E& e)
        {
           if (compare == value) {
               result = IsEqual(compare_to, e.value);
           }
        }

        virtual void visit(const MemberElement& e);
        virtual void visit(const IElement& e);

        bool get() const;
    };


}; // namespace refract

#endif // #ifndef REFRACT_COMPARABLEVISITOR_H
