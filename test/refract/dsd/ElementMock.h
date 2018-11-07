//
//  test/refract/dsd/ElementMock.h
//  test-librefract
//
//  Created by Thomas Jandecka on 06/09/2017
//  Copyright (c) 2017 Apiary Inc. All rights reserved.
//

#ifndef TEST_REFRACT_DSD_ELEMENT_MOCK_INCLUDED
#define TEST_REFRACT_DSD_ELEMENT_MOCK_INCLUDED

#include "refract/ElementIfc.h"
#include "refract/InfoElements.h"

#include "../../Tracker.h"

namespace refract
{
    namespace test
    {
        ///
        /// Simple mock of an element. No thread-safety guarantees.
        ///
        struct ElementMock : tracked<ElementMock>, refract::IElement {
            mutable int _total_ctx = 0;

            /// A value
            int _value = 42;

            mutable int meta_ctx = 0;
            refract::InfoElements meta_out;
            refract::InfoElements& meta() noexcept override
            {
                ++_total_ctx;
                ++meta_ctx;
                return meta_out;
            }

            mutable int meta_const_ctx = 0;
            refract::InfoElements meta_const_out;
            const refract::InfoElements& meta() const noexcept override
            {
                ++_total_ctx;
                ++meta_const_ctx;
                return meta_const_out;
            }

            mutable int attributes_ctx = 0;
            refract::InfoElements attributes_out;
            refract::InfoElements& attributes() noexcept override
            {
                ++_total_ctx;
                ++attributes_ctx;
                return attributes_out;
            }

            mutable int attributes_const_ctx = 0;
            refract::InfoElements attributes_const_out;
            const refract::InfoElements& attributes() const noexcept override
            {
                ++_total_ctx;
                ++attributes_const_ctx;
                return attributes_const_out;
            }

            mutable int element_ctx = 0;
            std::string element_out;
            std::string element() const override
            {
                ++_total_ctx;
                ++element_ctx;
                return element_out;
            }

            mutable int element_set_ctx = 0;
            std::string element_set_in = {};
            void element(const std::string& in) override
            {
                ++_total_ctx;
                ++element_set_ctx;
                element_set_in = in;
            }

            mutable int content_ctx = 0;
            mutable refract::Visitor* content_in = nullptr;
            void content(refract::Visitor& v) const override
            {
                ++_total_ctx;
                ++content_ctx;
                content_in = &v;
            }

            mutable int visit_ctx = 0;
            mutable refract::IVisitor* visit_in = nullptr;
            void visit(refract::IVisitor& v) const override
            {
                ++_total_ctx;
                ++visit_ctx;
                visit_in = &v;
            }

            mutable int clone_ctx = 0;
            mutable int clone_in = 0;
            mutable std::unique_ptr<refract::IElement> clone_out = {};
            std::unique_ptr<refract::IElement> clone(int flags) const override
            {
                ++_total_ctx;
                ++clone_ctx;
                clone_in = flags;
                return std::move(clone_out);
            }

            mutable int empty_ctx = 0;
            bool empty_out = false;
            bool empty() const override
            {
                ++_total_ctx;
                ++empty_ctx;
                return empty_out;
            }
        };
    }
}

#endif
