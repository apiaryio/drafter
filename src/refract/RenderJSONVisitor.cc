//
//  refract/RenderJSONVisitor.cc
//  librefract
//
//  Created by Pavan Kumar Sunkara on 17/06/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RenderJSONVisitor.h"

#include "SerializeCompactVisitor.h"
#include "VisitorUtils.h"
#include "PrintVisitor.h"

#include <sos.h>
#include <sosJSON.h>
#include <sstream>

namespace refract
{

    namespace
    {

        template <typename T, typename Collection>
        void FetchMembers(const T& element, Collection& members)
        {
            auto val = GetValue<T>()(element);

            if (!val) {
                return;
            }

            if (!val->empty())
                for (auto const& item : val->get()) {

                    if (!item || item->empty()) {
                        continue;
                    }

                    if (auto ref = TypeQueryVisitor::as<const RefElement>(item.get())) {
                        HandleRefWhenFetchingMembers<T>(*ref, members, FetchMembers<T, Collection>);
                        continue;
                    } else if (auto select = TypeQueryVisitor::as<const SelectElement>(item.get())) {
                        if (select->get().empty() || !(*select->get().begin())) {
                            continue;
                        }

                        FetchMembers(*(*select->get().begin()), members);
                        continue;
                    }

                    RenderJSONVisitor renderer;
                    Visit(renderer, *item);
                    auto e = renderer.getOwnership();

                    if (!e) {
                        continue;
                    }

                    members.push_back(std::move(e));
                }
        }
    }

    void RenderJSONVisitor::operator()(const IElement& e)
    {
        VisitBy(e, *this);
    }

    void RenderJSONVisitor::operator()(const MemberElement& e)
    {

        std::string key = GetKeyAsString(e);

        if (key.empty()) {
            return;
        }

        RenderJSONVisitor renderer;
        auto& content = e.get();

        if (content.value()) {
            if (auto enm = TypeQueryVisitor::as<const EnumElement>(content.value())) {
                // We need to handle Enum individualy because of attr["enumerations"]
                Visit(renderer, *enm);
            } else if (HasTypeAttribute(e, "nullable") && content.value()->empty()) {
                renderer.result = make_element<NullElement>();
            } else if (HasTypeAttribute(e, "optional") && content.value()->empty()) {
                return;
            } else {
                Visit(renderer, *content.value());
            }
        }

        if (!renderer.result) {
            return;
        }

        result = make_element<MemberElement>( //
            from_primitive(key),
            renderer.result ? renderer.getOwnership() : make_empty<StringElement>());
    }

    void RenderJSONVisitor::operator()(const ObjectElement& e)
    {
        ObjectElement::ValueType members;
        FetchMembers(e, members); // TODO XXX investigate this
        result = make_element<ObjectElement>(members);
    }

    void RenderJSONVisitor::operator()(const EnumElement& e)
    {

        const IElement* val = GetValue<EnumElement>()(e);

        std::unique_ptr<IElement> value = //
            (val && !val->empty()) ?      //
            val->clone() :                //
            make_empty<StringElement>();

        RenderJSONVisitor renderer;
        VisitBy(*value, renderer);
        result = renderer.getOwnership();
    }

    void RenderJSONVisitor::operator()(const ArrayElement& e)
    {
        ArrayElement::ValueType members;
        FetchMembers(e, members);
        result = make_element<ArrayElement>(std::move(members));
    }

    void RenderJSONVisitor::operator()(const NullElement& e)
    {
        result = make_empty<NullElement>();
    }

    template <typename T>
    std::unique_ptr<IElement> getResult(const T& e)
    {
        auto v = GetValue<T>{}(e);
        return v ?                      //
            (v->empty() ?               //
                    make_element<T>() : // default content if empty
                    make_element<T>(v->get())) :
            nullptr;
    }

    void RenderJSONVisitor::operator()(const StringElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const NumberElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const BooleanElement& e)
    {
        result = getResult(e);
    }

    void RenderJSONVisitor::operator()(const ExtendElement& e)
    {

        RenderJSONVisitor renderer;
        auto merged = e.get().merge();

        if (!merged) {
            return;
        }

        Visit(renderer, *merged);
        result = renderer.getOwnership();
    }

    std::unique_ptr<IElement> RenderJSONVisitor::getOwnership()
    {
        return std::move(result);
    }

    std::string RenderJSONVisitor::getString() const
    {
        std::string out;

        if (result) {
            sos::SerializeJSON serializer;
            std::stringstream ss;

            // FIXME: remove SosSerializeCompactVisitor dependency
            SosSerializeCompactVisitor s;
            VisitBy(*result, s);
            serializer.process(s.value(), ss);

            return ss.str();
        }

        return out;
    }
}
