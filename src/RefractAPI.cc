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

#include <iterator>
#include <set>

#include "NamedTypesRegistry.h"

namespace drafter {

    // Forward Declarations
    using refract::RefractElements;
    refract::IElement* ElementToRefract(const NodeInfo<snowcrash::Element>& element);

    namespace {

        template <typename T>
        bool IsNull(const T* ptr)
        {
            return ptr == NULL;
        }

        void RemoveEmptyElements(RefractElements& elements)
        {
            elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        }

        template <typename T, typename F>
        void NodeInfoToElements(const NodeInfo<T>& nodeInfo, const F& transformFunctor, RefractElements& content)
        {
            NodeInfoCollection<T> nodeInfoCollection(nodeInfo);
            std::transform(nodeInfoCollection.begin(), nodeInfoCollection.end(), std::back_inserter(content), transformFunctor);
        }

        template<typename T, typename C, typename F>
        T* CollectionToRefract(const NodeInfo<C>& collection, const F& transformFunctor, const std::string& key = std::string(), const refract::IElement::renderFlags renderType = refract::IElement::rCompact)
        {
            T* element = new T;
            RefractElements content;

            if (!key.empty()) {
                element->element(key);
            }

            NodeInfoToElements(collection, transformFunctor, content);

            RemoveEmptyElements(content);

            element->set(content);

            element->renderType(renderType);

            return element;
        }
    }

    refract::IElement* DataStructureToRefract(const NodeInfo<snowcrash::DataStructure>& dataStructure, bool expand)
    {
        refract::IElement* msonElement = MSONToRefract(dataStructure);

        if (expand) {
            refract::IElement* msonExpanded = ExpandRefract(msonElement, GetNamedTypesRegistry());
            msonElement = msonExpanded;
        }

        if (!msonElement) {
            return NULL;
        }

        refract::ObjectElement* element = new refract::ObjectElement;
        element->element(SerializeKey::DataStructure);
        element->push_back(msonElement);

        return element;
    }

    refract::IElement* MetadataToRefract(const NodeInfo<snowcrash::Metadata>& metadata)
    {
        refract::MemberElement* element = new refract::MemberElement;

        refract::ArrayElement* classes = CreateArrayElement(SerializeKey::User);
        classes->renderType(refract::IElement::rCompact);

        element->meta[SerializeKey::Classes] = classes;
        element->set(refract::IElement::Create(metadata.node->first), refract::IElement::Create(metadata.node->second));
        element->renderType(refract::IElement::rFull);

        AttachSourceMap(element, metadata);

        return element;
    }

    refract::IElement* CopyToRefract(const NodeInfo<std::string>& copy)
    {
        if (copy.node->empty()) {
            return NULL;
        }

        refract::IElement* element = PrimitiveToRefract(copy);
        element->element(SerializeKey::Copy);

        return element;
    }

    template<typename T>
    refract::IElement* ParameterValuesToRefract(const NodeInfo<snowcrash::Parameter>& parameter)
    {
        refract::ArrayElement* element = CollectionToRefract<refract::ArrayElement>(MAKE_NODE_INFO(parameter, values), LiteralToRefract<T>, SerializeKey::Enum, refract::IElement::rDefault);

        // FIXME: nearly duplicit code in ExtractParameter()
        // Add sample value
        if (!parameter.node->exampleValue.empty()) {
            refract::ArrayElement* samples = new refract::ArrayElement;
            samples->push_back(CreateArrayElement(LiteralToRefract<T>(MAKE_NODE_INFO(parameter, exampleValue)), true));
            element->attributes[SerializeKey::Samples] = samples;
        }

        // Add default value
        if (!parameter.node->defaultValue.empty()) {
            element->attributes[SerializeKey::Default] = CreateArrayElement(LiteralToRefract<T>(MAKE_NODE_INFO(parameter, defaultValue)), true);
        }

        return element;
    }

    template<typename T>
    refract::IElement* ExtractParameter(const NodeInfo<snowcrash::Parameter>& parameter)
    {
        refract::IElement* element = NULL;

        if (parameter.node->values.empty()) {
            element = LiteralToRefract<T>(MAKE_NODE_INFO(parameter, exampleValue));

            if (!parameter.node->defaultValue.empty()) {
                element->attributes[SerializeKey::Default] = PrimitiveToRefract(MAKE_NODE_INFO(parameter, defaultValue));
            }
        }
        else {
            element = ParameterValuesToRefract<T>(parameter);
        }

        return element;
    }

    refract::IElement* ParameterToRefract(const NodeInfo<snowcrash::Parameter>& parameter)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement *value = NULL;

        // Parameter type, exampleValue, defaultValue, values
        if (parameter.node->type == "boolean") {
            value = ExtractParameter<bool>(parameter);
        }
        else if (parameter.node->type == "number") {
            value = ExtractParameter<double>(parameter);
        }
        else {
            value = ExtractParameter<std::string>(parameter);
        }

        element->set(PrimitiveToRefract(MAKE_NODE_INFO(parameter, name)), value);

        // Description
        if (!parameter.node->description.empty()) {
            element->meta[SerializeKey::Description] = PrimitiveToRefract(MAKE_NODE_INFO(parameter, description));
        }

        // Parameter use
        if (parameter.node->use == snowcrash::RequiredParameterUse || parameter.node->use == snowcrash::OptionalParameterUse) {
            refract::ArrayElement* typeAttributes = new refract::ArrayElement;

            typeAttributes->push_back(refract::IElement::Create(parameter.node->use == snowcrash::RequiredParameterUse ? SerializeKey::Required : SerializeKey::Optional));
            element->attributes[SerializeKey::TypeAttributes] = typeAttributes;
        }

        return element;
    }

    refract::IElement* ParametersToRefract(const NodeInfo<snowcrash::Parameters>& parameters)
    {
        return CollectionToRefract<refract::ObjectElement>(parameters, ParameterToRefract, SerializeKey::HrefVariables, refract::IElement::rFull);
    }

    refract::IElement* HeaderToRefract(const NodeInfo<snowcrash::Header>& header)
    {
        refract::MemberElement* element = new refract::MemberElement;

        element->set(refract::IElement::Create(header.node->first), refract::IElement::Create(header.node->second));

        AttachSourceMap(element, header);

        return element;
    }

    refract::IElement* AssetToRefract(const NodeInfo<snowcrash::Asset>& asset, const std::string& contentType, const std::string&  metaClass)
    {
        if (asset.node->empty()) {
            return NULL;
        }

        refract::IElement* element = PrimitiveToRefract(asset);

        element->element(SerializeKey::Asset);
        element->meta[SerializeKey::Classes] = CreateArrayElement(metaClass);

        if (!contentType.empty()) {
            // FIXME: "contentType" has no sourceMap?
            element->attributes[SerializeKey::ContentType] = refract::IElement::Create(contentType);
        }

        return element;
    }

    refract::IElement* PayloadToRefract(const NodeInfo<snowcrash::Payload>& payload, const NodeInfo<snowcrash::Action>& action)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        // Use HTTP method to recognize if request or response
        if (action.isNull() || action.node->method.empty()) {
            element->element(SerializeKey::HTTPResponse);

            // FIXME: tests pass without commented out part of condition
            // delivery test to see this part is required else remove it
            // related discussion: https://github.com/apiaryio/drafter/pull/148/files#r42275194
            if (!payload.isNull() /* && !payload.node->name.empty() */) {
                element->attributes[SerializeKey::StatusCode] = PrimitiveToRefract(MAKE_NODE_INFO(payload, name));
            }
        }
        else {
            element->element(SerializeKey::HTTPRequest);
            element->attributes[SerializeKey::Method] = PrimitiveToRefract(MAKE_NODE_INFO(action, method));

            if (!payload.isNull()) {
                element->attributes[SerializeKey::Title] = PrimitiveToRefract(MAKE_NODE_INFO(payload, name));
            }
        }

        // If no payload, return immediately
        if (payload.isNull()) {
            element->set(content);
            return element;
        }

        if (!payload.node->headers.empty()) {
            element->attributes[SerializeKey::Headers] = CollectionToRefract<refract::ArrayElement>(MAKE_NODE_INFO(payload, headers),
                                                                                                    HeaderToRefract,
                                                                                                    SerializeKey::HTTPHeaders,
                                                                                                    refract::IElement::rFull);
        }

        // Render using boutique
        NodeInfoByValue<snowcrash::Asset> payloadBody = renderPayloadBody(payload, action, GetNamedTypesRegistry());
        NodeInfoByValue<snowcrash::Asset> payloadSchema = renderPayloadSchema(payload, action, GetNamedTypesRegistry());

        content.push_back(CopyToRefract(MAKE_NODE_INFO(payload, description)));
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(payload, attributes)));

        // Get content type
        std::string contentType = getContentTypeFromHeaders(payload.node->headers);

        // Assets
        content.push_back(AssetToRefract(NodeInfo<snowcrash::Asset>(payloadBody), contentType, SerializeKey::MessageBody));
        content.push_back(AssetToRefract(NodeInfo<snowcrash::Asset>(payloadSchema), contentType, SerializeKey::MessageBodySchema));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* TransactionToRefract(const NodeInfo<snowcrash::TransactionExample>& transaction,
                                            const NodeInfo<snowcrash::Action>& action,
                                            const NodeInfo<snowcrash::Request>& request,
                                            const NodeInfo<snowcrash::Response>& response)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::HTTPTransaction);
        content.push_back(CopyToRefract(MAKE_NODE_INFO(transaction, description)));

        content.push_back(PayloadToRefract(request, action));
        content.push_back(PayloadToRefract(response, NodeInfo<snowcrash::Action>()));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* ActionToRefract(const NodeInfo<snowcrash::Action>& action)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::Transition);
        element->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_NODE_INFO(action, name));

        if (!action.node->relation.str.empty()) {
            // We can't use PrimitiveToRefract() because `action.node->relation` here is a struct Relation
            refract::StringElement* relation = refract::IElement::Create(action.node->relation.str);
            AttachSourceMap(relation, MAKE_NODE_INFO(action, relation));

            element->attributes[SerializeKey::Relation] = relation;
        }

        if (!action.node->uriTemplate.empty()) {
            element->attributes[SerializeKey::Href] = PrimitiveToRefract(MAKE_NODE_INFO(action, uriTemplate));
        }

        if (!action.node->parameters.empty()) {
            element->attributes[SerializeKey::HrefVariables] = ParametersToRefract(MAKE_NODE_INFO(action, parameters));
        }

        if (!action.node->attributes.empty()) {
            refract::IElement* dataStructure = DataStructureToRefract(MAKE_NODE_INFO(action, attributes));
            dataStructure->renderType(refract::IElement::rFull);
            element->attributes[SerializeKey::Data] = dataStructure;
        }

        content.push_back(CopyToRefract(MAKE_NODE_INFO(action, description)));

        typedef NodeInfoCollection<snowcrash::TransactionExamples> ExamplesType;
        ExamplesType examples(MAKE_NODE_INFO(action, examples));

        for (ExamplesType::const_iterator it = examples.begin();
             it != examples.end();
             ++it) {

            // When there are only responses
            if (it->node->requests.empty() && !it->node->responses.empty()) {

                typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
                ResponsesType responses(it->node->responses, it->sourceMap->responses);

                for (ResponsesType::const_iterator resIt = responses.begin();
                     resIt != responses.end();
                     ++resIt) {

                    content.push_back(TransactionToRefract(*it, action, NodeInfo<snowcrash::Request>(), *resIt));
                }
            }

            // When there are only requests or both responses and requests
            typedef NodeInfoCollection<snowcrash::Requests> RequestsType;
            RequestsType requests(it->node->requests, it->sourceMap->requests);

            for (RequestsType::const_iterator reqIt = requests.begin();
                 reqIt != requests.end();
                 ++reqIt) {

                if (it->node->responses.empty()) {
                    content.push_back(TransactionToRefract(*it, action, *reqIt, NodeInfo<snowcrash::Response>()));
                }

                typedef NodeInfoCollection<snowcrash::Responses> ResponsesType;
                ResponsesType responses(it->node->responses, it->sourceMap->responses);

                for (ResponsesType::const_iterator resIt = responses.begin();
                     resIt != responses.end();
                     ++resIt) {

                    content.push_back(TransactionToRefract(*it, action, *reqIt, *resIt));
                }
            }
        }

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* ResourceToRefract(const NodeInfo<snowcrash::Resource>& resource)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::Resource);

        element->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_NODE_INFO(resource, name));

        element->attributes[SerializeKey::Href] = PrimitiveToRefract(MAKE_NODE_INFO(resource, uriTemplate));

        if (!resource.node->parameters.empty()) {
            element->attributes[SerializeKey::HrefVariables] = ParametersToRefract(MAKE_NODE_INFO(resource, parameters));
        }

        content.push_back(CopyToRefract(MAKE_NODE_INFO(resource, description)));
        content.push_back(DataStructureToRefract(MAKE_NODE_INFO(resource, attributes)));
        NodeInfoToElements(MAKE_NODE_INFO(resource, actions), ActionToRefract, content);

        RemoveEmptyElements(content);

        element->set(content);

        return element;
    }

    const snowcrash::SourceMap<snowcrash::Elements>* GetElementChildrenSourceMap(const NodeInfo<snowcrash::Element>& element)
    {
        return element.sourceMap->content.elements().collection.empty()
            ? NodeInfo<snowcrash::Elements>::NullSourceMap()
            : &element.sourceMap->content.elements();
    }

    refract::IElement* CategoryToRefract(const NodeInfo<snowcrash::Element>& element)
    {
        refract::ArrayElement* category = new refract::ArrayElement;
        RefractElements content;

        category->element(SerializeKey::Category);

        if (element.node->category == snowcrash::Element::ResourceGroupCategory) {
            category->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::ResourceGroup);
            category->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_NODE_INFO(element, attributes.name));
        }
        else if (element.node->category == snowcrash::Element::DataStructureGroupCategory) {
            category->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::DataStructures);
        }

        if (!element.node->content.elements().empty()) {
            const NodeInfo<snowcrash::Elements> elementsNodeInfo = MakeNodeInfo(&element.node->content.elements(), GetElementChildrenSourceMap(element), element.hasSourceMap());

            NodeInfoToElements(elementsNodeInfo, ElementToRefract, content);
        }

        RemoveEmptyElements(content);
        category->set(content);

        return category;
    }

    refract::IElement* ElementToRefract(const NodeInfo<snowcrash::Element>& element)
    {
        switch (element.node->element) {
            case snowcrash::Element::ResourceElement:
                return ResourceToRefract(MAKE_NODE_INFO(element, content.resource));
            case snowcrash::Element::DataStructureElement:
                return DataStructureToRefract(MAKE_NODE_INFO(element, content.dataStructure));
            case snowcrash::Element::CopyElement:
                return CopyToRefract(MAKE_NODE_INFO(element, content.copy));
            case snowcrash::Element::CategoryElement:
                return CategoryToRefract(element);
            default:
                throw snowcrash::Error("unknown type of api description element", snowcrash::ApplicationError);
        }
    }

    refract::IElement* BlueprintToRefract(const NodeInfo<snowcrash::Blueprint>& blueprint)
    {
        refract::ArrayElement* ast = new refract::ArrayElement;
        RefractElements content;

        ast->element(SerializeKey::Category);

        ast->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::API);
        ast->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_NODE_INFO(blueprint, name));

        content.push_back(CopyToRefract(MAKE_NODE_INFO(blueprint, description)));

        if (!blueprint.node->metadata.empty()) {
            ast->attributes[SerializeKey::Meta] = CollectionToRefract<refract::ArrayElement>(MAKE_NODE_INFO(blueprint, metadata), MetadataToRefract);
        }

        NodeInfoToElements(MAKE_NODE_INFO(blueprint, content.elements()), ElementToRefract, content);

        RemoveEmptyElements(content);
        ast->set(content);

        return ast;
    }

    refract::IElement* AnnotationToRefract(const snowcrash::SourceAnnotation& annotation, const std::string& key)
    {
        refract::IElement* element = refract::IElement::Create(annotation.message);

        element->element(SerializeKey::Annotation);

        element->meta[SerializeKey::Classes] = CreateArrayElement(key);

        element->attributes[SerializeKey::AnnotationCode] = refract::IElement::Create(annotation.code);
        element->attributes[SerializeKey::SourceMap] = SourceMapToRefract(annotation.location);

        return element;
    }

} // namespace drafter
