//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "SourceAnnotation.h"

#include "RefractDataStructure.h"
#include "RefractAPI.h"
#include "Render.h"
#include "RefractSourceMap.h"

#include "refract/Exception.h"
#include "refract/JsonValue.h"
#include "refract/JsonSchema.h"

#include "utils/log/Trivial.h"
#include "utils/so/JsonIo.h"

#include "apib2apie/CollectionToApie.h"
#include "apib2apie/CopyToApie.h"
#include "apib2apie/ParametersToApie.h"
#include "apib2apie/PayloadToApie.h"

#include <iterator>
#include <set>

#include "NamedTypesRegistry.h"
#include "ConversionContext.h"

using namespace drafter;
using namespace refract;
using namespace drafter::utils::log;

// Forward Declarations
std::unique_ptr<IElement> ElementToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context);

namespace
{
    struct is_nullptr {
        template <typename T>
        constexpr bool operator()(const T* ptr) const noexcept
        {
            return ptr == nullptr;
        }

        template <typename T>
        constexpr bool operator()(const std::unique_ptr<T>& ptr) const noexcept
        {
            return ptr == nullptr;
        }
    };

    template <typename Collection>
    void RemoveEmptyElements(Collection& elements)
    {
        elements.erase(std::remove_if(elements.begin(), elements.end(), is_nullptr{}), elements.end());
    }

    template <typename T, typename DataT, typename Functor>
    void NodeInfoToElements(const NodeInfo<T>& nodeInfo, const Functor& f, DataT& content, ConversionContext& context)
    {
        NodeInfoCollection<T> nodeInfoCollection(nodeInfo);

        std::transform(nodeInfoCollection.begin(),
            nodeInfoCollection.end(),
            std::back_inserter(content),
            [&f, &context](const typename NodeInfoCollection<T>::value_type& nodeInfo) { //
                return f(nodeInfo, context);
            });
    }

} // namespace

std::unique_ptr<IElement> drafter::DataStructureToRefract(
    const NodeInfo<snowcrash::DataStructure>& dataStructure, ConversionContext& context)
{
    // TODO: Check for already expanded MSON in context.registry and use it if possible.
    // We aren't doing it yet because APIB AST with MSON Refract will start getting sourcemaps
    // which is a breaking change. Once we remove APIB AST code, we can move forward with this.
    auto msonElement = MSONToRefract(dataStructure, context);

    if (context.expandMson()) {
        auto msonExpanded = ExpandRefract(std::move(msonElement), context);
        msonElement = std::move(msonExpanded);
    }

    return msonElement ?                                                                                      //
        refract::make_unique<HolderElement>(SerializeKey::DataStructure, dsd::Holder(std::move(msonElement))) //
        :
        nullptr;
}

std::unique_ptr<IElement> MetadataToRefract(const NodeInfo<snowcrash::Metadata>& metadata, ConversionContext& context)
{
    auto element = make_element<MemberElement>( //
        from_primitive(metadata.node->first),
        from_primitive(metadata.node->second));

    element->meta().set(SerializeKey::Classes,
        generate_element<ArrayElement>([](dsd::Array& data) { //
            data.push_back(from_primitive(SerializeKey::User));
        }));

    AttachSourceMap(*element, metadata);

    return std::move(element);
}

std::unique_ptr<ArrayElement> TransactionToRefract(const NodeInfo<snowcrash::TransactionExample>& transaction,
    const NodeInfo<snowcrash::Action>& action,
    const NodeInfo<snowcrash::Request>& request,
    const NodeInfo<snowcrash::Response>& response,
    ConversionContext& context)
{
    auto element = make_element<ArrayElement>();
    auto& content = element->get();

    element->element(SerializeKey::HTTPTransaction);

    if (!transaction.node->description.empty())
        content.push_back(apib2apie::CopyToApie(MAKE_NODE_INFO(transaction, description)));
    content.push_back(apib2apie::PayloadToApie(request, action, context));
    content.push_back(apib2apie::PayloadToApie(response, NodeInfo<snowcrash::Action>(), context));

    RemoveEmptyElements(content);

    return element;
}

std::unique_ptr<ArrayElement> ActionToRefract(const NodeInfo<snowcrash::Action>& action, ConversionContext& context)
{
    auto element = make_element<ArrayElement>();

    element->element(SerializeKey::Transition);
    element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(action, name)));

    if (!action.node->relation.str.empty()) {
        // We can't use PrimitiveToRefract() because `action.node->relation` here is a struct Relation
        auto relation = from_primitive(action.node->relation.str);
        AttachSourceMap(*relation, MAKE_NODE_INFO(action, relation));
        element->attributes().set(SerializeKey::Relation, std::move(relation));
    }

    if (!action.node->uriTemplate.empty()) {
        element->attributes().set(SerializeKey::Href, PrimitiveToRefract(MAKE_NODE_INFO(action, uriTemplate)));
    }

    if (!action.node->parameters.empty()) {
        element->attributes().set( //
            SerializeKey::HrefVariables,
            apib2apie::ParametersToApie(MAKE_NODE_INFO(action, parameters), context));
    }

    if (!action.node->attributes.empty()) {
        element->attributes().set(
            SerializeKey::Data, DataStructureToRefract(MAKE_NODE_INFO(action, attributes), context));
    }

    auto& content = element->get();

    if (!action.node->description.empty())
        content.push_back(apib2apie::CopyToApie(MAKE_NODE_INFO(action, description)));

    typedef NodeInfoCollection<snowcrash::TransactionExamples> ExamplesType;
    ExamplesType examples(MAKE_NODE_INFO(action, examples));

    for (const auto& example : examples) {

        // When there are only responses
        if (example.node->requests.empty() && !example.node->responses.empty()) {

            typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
            ResponsesType responses(example.node->responses, example.sourceMap->responses);

            for (const auto& response : responses) {
                content.push_back(
                    TransactionToRefract(example, action, NodeInfo<snowcrash::Request>(), response, context));
            }
        }

        // When there are only requests or both responses and requests
        typedef NodeInfoCollection<snowcrash::Requests> RequestsType;
        RequestsType requests(example.node->requests, example.sourceMap->requests);

        for (const auto& request : requests) {

            if (example.node->responses.empty()) {
                content.push_back(
                    TransactionToRefract(example, action, request, NodeInfo<snowcrash::Response>(), context));
            }

            typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
            ResponsesType responses(example.node->responses, example.sourceMap->responses);

            for (const auto& response : responses) {
                content.push_back(TransactionToRefract(example, action, request, response, context));
            }
        }
    }

    RemoveEmptyElements(content);

    return element;
}

std::unique_ptr<ArrayElement> ResourceToRefract(
    const NodeInfo<snowcrash::Resource>& resource, ConversionContext& context)
{
    auto element = make_element<ArrayElement>();

    element->element(SerializeKey::Resource);

    element->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(resource, name)));
    element->attributes().set(SerializeKey::Href, PrimitiveToRefract(MAKE_NODE_INFO(resource, uriTemplate)));

    if (!resource.node->parameters.empty()) {
        element->attributes().set( //
            SerializeKey::HrefVariables,
            apib2apie::ParametersToApie(MAKE_NODE_INFO(resource, parameters), context));
    }

    auto& content = element->get();

    if (!resource.node->description.empty())
        content.push_back(apib2apie::CopyToApie(MAKE_NODE_INFO(resource, description)));

    if (!resource.node->attributes.empty()) {
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(resource, attributes), context));
    }
    NodeInfoToElements(MAKE_NODE_INFO(resource, actions), ActionToRefract, content, context);

    RemoveEmptyElements(content);

    return element;
}

const snowcrash::SourceMap<snowcrash::Elements>* GetElementChildrenSourceMap(
    const NodeInfo<snowcrash::Element>& element)
{
    return element.sourceMap->content.elements().collection.empty() ? NodeInfo<snowcrash::Elements>::NullSourceMap() :
                                                                      &element.sourceMap->content.elements();
}

std::unique_ptr<ArrayElement> CategoryToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context)
{
    auto category = make_element<ArrayElement>();

    category->element(SerializeKey::Category);

    if (element.node->category == snowcrash::Element::ResourceGroupCategory) {
        category->meta().set(
            SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::ResourceGroup)));
        category->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(element, attributes.name)));
    } else if (element.node->category == snowcrash::Element::DataStructureGroupCategory) {
        category->meta().set(
            SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::DataStructures)));
    }

    auto& content = category->get();

    if (!element.node->content.elements().empty()) {
        const NodeInfo<snowcrash::Elements> elementsNodeInfo
            = MakeNodeInfo(&element.node->content.elements(), GetElementChildrenSourceMap(element));

        NodeInfoToElements(elementsNodeInfo, ElementToRefract, content, context);
    }

    RemoveEmptyElements(content);

    return category;
}

std::unique_ptr<IElement> ElementToRefract(const NodeInfo<snowcrash::Element>& element, ConversionContext& context)
{
    switch (element.node->element) {
        case snowcrash::Element::ResourceElement:
            return ResourceToRefract(MAKE_NODE_INFO(element, content.resource), context);
        case snowcrash::Element::DataStructureElement:
            return DataStructureToRefract(MAKE_NODE_INFO(element, content.dataStructure), context);
        case snowcrash::Element::CopyElement:
            return apib2apie::CopyToApie(MAKE_NODE_INFO(element, content.copy));
        case snowcrash::Element::CategoryElement:
            return CategoryToRefract(element, context);
        default:
            // we are not able to get sourcemap info there
            // It is strongly dependent on type of element.
            // For unknown type of Element we are not able to locate SourceMap
            // with adequate effort we do not provide it to upper level
            throw snowcrash::Error("unknown type of api description element", snowcrash::ApplicationError);
    }
}

std::unique_ptr<IElement> drafter::BlueprintToRefract(
    const NodeInfo<snowcrash::Blueprint>& blueprint, ConversionContext& context)
{
    auto ast = make_element<ArrayElement>();

    ast->element(SerializeKey::Category);

    ast->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(SerializeKey::API)));
    ast->meta().set(SerializeKey::Title, PrimitiveToRefract(MAKE_NODE_INFO(blueprint, name)));

    auto& content = ast->get();

    if (!blueprint.node->description.empty())
        content.push_back(apib2apie::CopyToApie(MAKE_NODE_INFO(blueprint, description)));

    if (!blueprint.node->metadata.empty()) {
        ast->attributes().set(SerializeKey::Metadata,
            apib2apie::CollectionToApie<ArrayElement>(MAKE_NODE_INFO(blueprint, metadata), context, MetadataToRefract));
    }

    NodeInfoToElements(MAKE_NODE_INFO(blueprint, content.elements()), ElementToRefract, content, context);

    RemoveEmptyElements(content);

    return std::move(ast);
}

std::unique_ptr<IElement> drafter::AnnotationToRefract(
    const snowcrash::SourceAnnotation& annotation, const std::string& key, ConversionContext& context)
{
    auto element = from_primitive(annotation.message);

    element->element(SerializeKey::Annotation);

    element->meta().set(SerializeKey::Classes, make_element<ArrayElement>(from_primitive(key)));

    element->attributes().set(SerializeKey::AnnotationCode, from_primitive(annotation.code));
    element->attributes().set(
        SerializeKey::SourceMap, SourceMapToRefractWithColumnLineInfo(annotation.location, context));

    return std::move(element);
}
