//
//  refract/FilterVisitor.h
//  librefract
//
//  Created by Jiri Kratochvil on 10/06/16.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//
#ifndef REFRACT_FILTERVISITOR_H
#define REFRACT_FILTERVISITOR_H

#include "ElementFwd.h"
#include "Visitor.h"

#include "Query.h"
#include <vector>

namespace refract
{

    class FilterVisitor
    {
        typedef std::vector<const IElement*> Elements;

        Elements selected;
        query::Query query;

        public:

        template <typename Predicate>
        FilterVisitor(Predicate predicate) : query(predicate) {};

        template <typename T>
        void operator()(const T& e){

            if(query(e)) {
                selected.push_back(&e);
            }
        };

        bool empty() const {
            return selected.empty();
        }

        const Elements& elements() const {
            return selected;
        }

    };

}; // namespace refract

#endif // #ifndef REFRACT_FILTERVISITOR_H
