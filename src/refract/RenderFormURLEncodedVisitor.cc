//
//  RenderFormURLEncodedVisitor.cc
//  librefract
//
//  Created by Kyle Fuller on 17/09/16.
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//

#include <iomanip>
#include "VisitorUtils.h"

#include "RenderFormURLEncodedVisitor.h"

namespace refract
{

    namespace {
        template <typename T>
        void IncludeMembers(const T& element, typename T::ValueType& members)
        {
            const typename T::ValueType* value = GetValue<T>(element);

            if (!value) {
                return;
            }

            for (typename T::ValueType::const_iterator it = value->begin();
                 it != value->end();
                 ++it) {

                if (!(*it) || (*it)->empty()) {
                    continue;
                }

                if ((*it)->element() == "ref") {
                    HandleRefWhenFetchingMembers<T>(*it, members, IncludeMembers<T>);
                    continue;
                }

                members.push_back(*it);
            }
        }
    }

    RenderFormURLEncodedVisitor::RenderFormURLEncodedVisitor(const std::string &key) : baseKey(key) {
    }

    RenderFormURLEncodedVisitor::~RenderFormURLEncodedVisitor() {
    }

    void RenderFormURLEncodedVisitor::operator()(const IElement& element) {
        VisitBy(element, *this);
    }

    void RenderFormURLEncodedVisitor::operator()(const MemberElement& element) {
        if (!element.value.first) {
            return;
        }

        StringElement *keyElement = static_cast<StringElement*>(element.value.first);
        if (!keyElement) {
            return;
        }

        const StringElement::ValueType* key = GetValue<StringElement>(*keyElement);
        if (!key) {
            return;
        }

        std::string encodedKey = percentEncode(*key);

        if (!result.str().empty()) {
            result << "&";
        }

        if (element.value.second) {
            if (!baseKey.empty()) {
                encodedKey = baseKey + "[" + encodedKey + "]";
            }

            RenderFormURLEncodedVisitor visitor(encodedKey);
            Visit(visitor, *element.value.second);
            result << visitor.getString();
        } else {
            result << encodedKey << "=";
        }
    }

    void RenderFormURLEncodedVisitor::operator()(const ObjectElement& element) {
        ObjectElement::ValueType value;
        IncludeMembers(element, value);

        for (std::vector<refract::IElement*>::const_iterator it = value.begin();
             it != value.end();
             ++it) {

            if (!*it) {
                continue;
            }

            MemberElement *member = static_cast<MemberElement*>(*it);
            if (member) {
               operator()(*member);
            }
        }
    }

    void RenderFormURLEncodedVisitor::operator()(const EnumElement& element) {
        const EnumElement::ValueType *value = GetEnumValue(element);

        if (value && !value->empty()) {
            operator()(*value->front());
        }

    }

    void RenderFormURLEncodedVisitor::operator()(const ArrayElement& element) {
        const ArrayElement::ValueType* value = GetValue<ArrayElement>(element);

        if (value) {
            for (ArrayElement::ValueType::const_iterator it = value->begin();
                 it != value->end();
                 ++it) {
                if (!*it) {
                    continue;
                }

                if (!result.str().empty()) {
                    result << "&";
                }

                RenderFormURLEncodedVisitor visitor(baseKey + "[]");
                IElement *itElement = *it;
                Visit(visitor, *itElement);
                result << visitor.getString();
            }
        }
    }

    void RenderFormURLEncodedVisitor::operator()(const NullElement& element) {
    }

    void RenderFormURLEncodedVisitor::operator()(const StringElement& element) {
        const StringElement::ValueType* value = GetValue<StringElement>(element);

        if (value) {
            if (!result.str().empty()) {
                result << "&";
            }

            result << baseKey << "=" << percentEncode(*value);
        }
    }

    template<typename T>
    void RenderFormURLEncodedVisitor::primitiveType(const T& element) {
        const typename T::ValueType *value = GetValue<T>(element);

        if (value) {
            if (!result.str().empty()) {
                result << "&";
            }

            result << baseKey << "=" << *value;
        }
    }

    void RenderFormURLEncodedVisitor::operator()(const NumberElement& element) {
        primitiveType(element);
    }

    void RenderFormURLEncodedVisitor::operator()(const BooleanElement& element) {
        primitiveType(element);
    }

    void RenderFormURLEncodedVisitor::operator()(const ExtendElement& element) {
    }

    std::string RenderFormURLEncodedVisitor::getString() const {
        return result.str();
    }

    std::string RenderFormURLEncodedVisitor::percentEncode(const std::string& value) const {
        std::stringstream encodedValue;

        for (std::string::const_iterator it = value.begin(), end = value.end(); it != end; ++it) {
            std::string::value_type character = (*it);

            if (isalnum(character) || character == '-' || character == '_' || character == '.' || character == '~') {
                encodedValue << character;
            } else {
                encodedValue << '%' << std::hex << int(character);
            }
        }

        return encodedValue.str();
    }
}
