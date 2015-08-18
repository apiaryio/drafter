//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "RefractDataStructure.h"
#include "RefractAPI.h"
#include "Render.h"

namespace drafter {

    // Forward Declarations
    refract::IElement* ElementToRefract(const snowcrash::Element& element);

    namespace {

        typedef std::vector<refract::IElement*> RefractElements;

        template <typename T>
        refract::ArrayElement* CreateArrayElement(const T& content)
        {
            refract::ArrayElement* array = new refract::ArrayElement;
            array->push_back(refract::IElement::Create(content));
            return array;
        }

        template <typename T>
        bool IsNull(const T* ptr)
        {
            return ptr == NULL;
        }

        void RemoveEmptyElements(RefractElements& elements)
        {
            elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        }
    }

    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure)
    {
        refract::IElement* msonElement = MSONToRefract(dataStructure);

        if (!msonElement) {
            return NULL;
        }

        refract::ObjectElement* element = new refract::ObjectElement;
        element->element("dataStructure");
        element->push_back(msonElement);

        return element;
    }

    refract::IElement* MetadataToRefract(const snowcrash::Metadata metadata)
    {
        refract::MemberElement* element = new refract::MemberElement;

        element->meta["classes"] = CreateArrayElement("user");
        element->meta["classes"].renderType(refract::IElement::rCompact);
        element->set(refract::IElement::Create(metadata.first), refract::IElement::Create(metadata.second));
        element->renderType(refract::IElement::rFull);

        return element;
    }

    refract::IElement* MetadataCollectionToRefract(const snowcrash::MetadataCollection metadata)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        std::transform(metadata.begin(), metadata.end(), std::back_inserter(content), MetadataToRefract);
        element->set(content);
        element->renderType(refract::IElement::rCompact);

        return element;
    }

    refract::IElement* CopyToRefract(const std::string& copy)
    {
        if (copy.empty()) {
            return NULL;
        }

        refract::IElement* element = refract::IElement::Create(copy);
        element->element("copy");

        return element;
    }

    template<typename T>
    refract::IElement* ParameterValuesToRefract(const snowcrash::Parameter& parameter)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("enum");

        if (!parameter.exampleValue.empty()) {
            element->attributes["samples"] = CreateArrayElement(LiteralTo<T>(parameter.exampleValue));
        }

        for (snowcrash::Values::const_iterator it = parameter.values.begin();
             it != parameter.values.end();
             ++it) {

            content.push_back(refract::IElement::Create(LiteralTo<T>(*it)));
        }

        element->set(content);

        return element;
    }

    template<typename T>
    refract::IElement* ExtractParameter(const snowcrash::Parameter& parameter)
    {
        refract::IElement* element = NULL;

        if (parameter.values.empty()) {
            element = refract::IElement::Create(LiteralTo<T>(parameter.exampleValue));
        }
        else {
            element = ParameterValuesToRefract<T>(parameter);
        }

        if (!parameter.defaultValue.empty()) {
            element->attributes["default"] = refract::IElement::Create(LiteralTo<T>(parameter.defaultValue));
        }

        return element;
    }

    refract::IElement* ParameterToRefract(const snowcrash::Parameter& parameter)
    {
        refract::MemberElement* element = new refract::MemberElement;
        refract::IElement *value = NULL;

        // Parameter type, exampleValue, defaultValue, values
        if (parameter.type == "boolean") {
            value = ExtractParameter<bool>(parameter);
        }
        else if (parameter.type == "number") {
            value = ExtractParameter<double>(parameter);
        }
        else {
            value = ExtractParameter<std::string>(parameter);
        }

        element->set(refract::IElement::Create(parameter.name), value);

        // Description
        if (!parameter.description.empty()) {
            element->meta["description"] = refract::IElement::Create(parameter.description);
        }

        // Parameter use
        if (parameter.use == snowcrash::RequiredParameterUse || parameter.use == snowcrash::OptionalParameterUse) {
            refract::ArrayElement* typeAttributes = new refract::ArrayElement;

            typeAttributes->push_back(refract::IElement::Create(parameter.use == snowcrash::RequiredParameterUse ? "required" : "optional"));
            element->attributes["typeAttributes"] = typeAttributes;
        }

        return element;
    }

    refract::IElement* ParametersToRefract(const snowcrash::Parameters& parameters)
    {
        refract::ObjectElement* element = new refract::ObjectElement;
        RefractElements content;

        element->element("hrefVariables");
        std::transform(parameters.begin(), parameters.end(), std::back_inserter(content), ParameterToRefract);
        element->renderType(refract::IElement::rFull);

        element->set(content);

        return element;
    }

    refract::IElement* HeaderToRefract(const snowcrash::Header& header)
    {
        refract::MemberElement* element = new refract::MemberElement;
        element->set(refract::IElement::Create(header.first), refract::IElement::Create(header.second));

        return element;
    }

    refract::IElement* HeadersToRefract(const snowcrash::Headers& headers)
    {
        refract::ObjectElement* element = new refract::ObjectElement;
        RefractElements content;

        element->element("httpHeaders");
        std::transform(headers.begin(), headers.end(), std::back_inserter(content), HeaderToRefract);
        element->renderType(refract::IElement::rFull);

        element->set(content);

        return element;
    }

    refract::IElement* AssetToRefract(const snowcrash::Asset& asset, const std::string& contentType, bool messageBody = true)
    {
        if (asset.empty()) {
            return NULL;
        }

        refract::IElement* element = refract::IElement::Create(asset);

        element->element("asset");
        element->meta["classes"] = refract::ArrayElement::Create(messageBody ? "messageBody" : "messageSchema");

        if (!contentType.empty()) {
            element->attributes["contentType"] = refract::IElement::Create(contentType);
        }

        return element;
    }

    refract::IElement* PayloadToRefract(const snowcrash::Payload* payload, const snowcrash::HTTPMethod& method = "")
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        // Use HTTP method to recognize if request or response
        if (method.empty()) {
            element->element("httpResponse");

            if (payload != NULL && !payload->name.empty()) {
                element->attributes["statusCode"] = refract::IElement::Create(payload->name);
            }
        }
        else {
            element->element("httpRequest");
            element->attributes["method"] = refract::IElement::Create(method);

            if (payload != NULL) {
                element->attributes["title"] = refract::IElement::Create(payload->name);
            }
        }

        // If no payload, return immediately
        if (payload == NULL) {
            element->set(content);
            return element;
        }

        if (!payload->headers.empty()) {
            element->attributes["headers"] = HeadersToRefract(payload->headers);
        }

        // Render using boutique
        snowcrash::Asset payloadBody = renderPayloadBody(*payload, NamedTypesRegistry);
        snowcrash::Asset payloadSchema = renderPayloadSchema(*payload);

        content.push_back(CopyToRefract(payload->description));
        content.push_back(DataStructureToRefract(payload->attributes));

        // Get content type
        std::string contentType = getContentTypeFromHeaders(payload->headers);

        // Assets
        content.push_back(AssetToRefract(payloadBody, contentType));
        content.push_back(AssetToRefract(payloadSchema, contentType, false));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* TransactionToRefract(const snowcrash::TransactionExample& transaction,
                                            const snowcrash::HTTPMethod& method,
                                            const snowcrash::Request* request = NULL,
                                            const snowcrash::Response* response = NULL)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("httpTransaction");
        content.push_back(CopyToRefract(transaction.description));

        content.push_back(PayloadToRefract(request, method));
        content.push_back(PayloadToRefract(response));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* ActionToRefract(const snowcrash::Action& action)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("transition");
        element->meta["title"] = refract::IElement::Create(action.name);

        if (!action.relation.str.empty()) {
            element->attributes["relation"] = refract::IElement::Create(action.relation.str);
        }

        if (!action.uriTemplate.empty()) {
            element->attributes["href"] = refract::IElement::Create(action.uriTemplate);
        }

        if (!action.parameters.empty()) {
            element->attributes["hrefVariables"] = ParametersToRefract(action.parameters);
        }

        if (!action.attributes.empty()) {
            refract::IElement* dataStructure = DataStructureToRefract(action.attributes);
            dataStructure->renderType(refract::IElement::rFull);
            element->attributes["data"] = dataStructure;
        }

        content.push_back(CopyToRefract(action.description));

        for (snowcrash::TransactionExamples::const_iterator it = action.examples.begin();
             it != action.examples.end();
             ++it) {

            // When there are only responses
            if (it->requests.empty() && !it->responses.empty()) {

                for (snowcrash::Responses::const_iterator tmpResIt = it->responses.begin();
                     tmpResIt != it->responses.end();
                     ++tmpResIt) {

                    content.push_back(TransactionToRefract(*it, action.method, NULL, &*tmpResIt));
                }
            }

            // When there are only requests or both responses and requests
            for (snowcrash::Requests::const_iterator reqIt = it->requests.begin();
                 reqIt != it->requests.end();
                 ++reqIt) {

                if (it->responses.empty()) {
                    content.push_back(TransactionToRefract(*it, action.method, &*reqIt));
                }

                for (snowcrash::Responses::const_iterator resIt = it->responses.begin();
                     resIt != it->responses.end();
                     ++resIt) {

                    content.push_back(TransactionToRefract(*it, action.method, &*reqIt, &*resIt));
                }
            }
        }

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* ResourceToRefract(const snowcrash::Resource& resource)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("resource");
        element->meta["title"] = refract::IElement::Create(resource.name);
        element->attributes["href"] = refract::IElement::Create(resource.uriTemplate);

        if (!resource.parameters.empty()) {
            element->attributes["hrefVariables"] = ParametersToRefract(resource.parameters);
        }

        content.push_back(CopyToRefract(resource.description));
        content.push_back(DataStructureToRefract(resource.attributes));

        std::transform(resource.actions.begin(), resource.actions.end(), std::back_inserter(content), ActionToRefract);

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* CategoryToRefract(const snowcrash::Element& element)
    {
        refract::ArrayElement* category = new refract::ArrayElement;
        RefractElements content;

        category->element("category");

        if (element.category == snowcrash::Element::ResourceGroupCategory) {
            category->meta["classes"] = CreateArrayElement("resourceGroup");
            category->meta["title"] = refract::IElement::Create(element.attributes.name);
        }
        else if (element.category == snowcrash::Element::DataStructureGroupCategory) {
            category->meta["classes"] = CreateArrayElement("dataStructures");
        }

        snowcrash::Elements elements = element.content.elements();
        std::transform(elements.begin(), elements.end(), std::back_inserter(content), ElementToRefract);

        RemoveEmptyElements(content);
        category->set(content);

        return category;
    }

    refract::IElement* ElementToRefract(const snowcrash::Element& element)
    {
        switch (element.element) {
            case snowcrash::Element::ResourceElement:
                return ResourceToRefract(element.content.resource);
            case snowcrash::Element::DataStructureElement:
                return DataStructureToRefract(element.content.dataStructure);
            case snowcrash::Element::CopyElement:
                return CopyToRefract(element.content.copy);
            case snowcrash::Element::CategoryElement:
                return CategoryToRefract(element);
            default:
                throw std::runtime_error("Not Implemented - Unhandled type of Element");
        }
    }

    refract::IElement* BlueprintToRefract(const snowcrash::Blueprint& blueprint)
    {
        refract::ArrayElement* ast = new refract::ArrayElement;
        RefractElements content;

        ast->element("category");
        ast->meta["classes"] = CreateArrayElement("api");
        ast->meta["title"] = refract::IElement::Create(blueprint.name);

        content.push_back(CopyToRefract(blueprint.description));

        if (!blueprint.metadata.empty()) {
            ast->attributes["meta"] = MetadataCollectionToRefract(blueprint.metadata);
        }

        // Append set of elements to content
        const snowcrash::Elements& scElements = blueprint.content.elements();
        std::transform(scElements.begin(), scElements.end(), std::back_inserter(content), ElementToRefract);

        RemoveEmptyElements(content);
        ast->set(content);

        return ast;
    }

} // namespace drafter
