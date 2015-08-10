//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "StringUtility.h"
#include "BlueprintUtility.h"

#include "RefractAPI.h"
#include "refract/Element.h"
#include "refract/Registry.h"
#include "refract/Visitors.h"

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
        refract::ObjectElement* element = new refract::ObjectElement;
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

    refract::IElement* ParameterToRefract(const snowcrash::Parameter parameter)
    {
        //FIXME: description, defaultValue, use, type
        refract::MemberElement* element = new refract::MemberElement;
        element->set(refract::IElement::Create(parameter.name), refract::IElement::Create(parameter.exampleValue));

        return element;
    }

    refract::IElement* ParametersToRefract(const snowcrash::Parameters& parameters)
    {
        refract::ObjectElement* element = new refract::ObjectElement;
        RefractElements content;

        element->element("hrefVariables");
        std::transform(parameters.begin(), parameters.end(), std::back_inserter(content), ParameterToRefract);

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
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("httpHeaders");
        std::transform(headers.begin(), headers.end(), std::back_inserter(content), HeaderToRefract);

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

    refract::IElement* PayloadToRefract(const snowcrash::Payload& payload, const snowcrash::HTTPMethod& method = "")
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        // Use HTTP method to recognize if request or response
        if (method.empty()) {
            element->element("httpResponse");

            if (!payload.name.empty()) {
                element->attributes["statusCode"] = refract::IElement::Create(payload.name);
            }
        }
        else {
            element->element("httpRequest");
            element->attributes["method"] = refract::IElement::Create(method);

            // FIXME: Expand href
        }

        if (!payload.headers.empty()) {
            element->attributes["headers"] = HeadersToRefract(payload.headers);
        }

        content.push_back(CopyToRefract(payload.description));
        content.push_back(DataStructureToRefract(payload.attributes));

        // Get content type
        std::string contentType = "";
        snowcrash::Headers::const_iterator it;

        it = std::find_if(payload.headers.begin(),
                          payload.headers.end(),
                          std::bind2nd(snowcrash::MatchFirstWith<snowcrash::Header, std::string, snowcrash::IEqual<std::string> >(),
                                       snowcrash::HTTPHeaderName::ContentType));

        if (it != payload.headers.end()) {
            contentType = it->second;
        }

        // Assets
        content.push_back(AssetToRefract(payload.body, contentType));
        content.push_back(AssetToRefract(payload.schema, contentType, false));

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
        element->set(content);

        return element;
    }

    refract::IElement* TransactionToRefract(const snowcrash::TransactionExample& transaction,
                                            const snowcrash::Request& request,
                                            const snowcrash::Response& response,
                                            const snowcrash::HTTPMethod& method)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element("httpTransaction");
        content.push_back(CopyToRefract(transaction.description));

        content.push_back(PayloadToRefract(request, method));
        content.push_back(PayloadToRefract(response));

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
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
            element->attributes["data"] = DataStructureToRefract(action.attributes);
        }

        content.push_back(CopyToRefract(action.description));

        for (snowcrash::TransactionExamples::const_iterator it = action.examples.begin();
             it != action.examples.end();
             ++it) {

            for (snowcrash::Requests::const_iterator reqIt = it->requests.begin();
                 reqIt != it->requests.end();
                 ++reqIt) {

                for (snowcrash::Responses::const_iterator resIt = it->responses.begin();
                     resIt != it->responses.end();
                     ++resIt) {

                    content.push_back(TransactionToRefract(*it, *reqIt, *resIt, action.method));
                }
            }
        }

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
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

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
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

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
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

        // Remove NULL elements
        content.erase(std::remove_if(content.begin(), content.end(), IsNull<refract::IElement>), content.end());
        ast->set(content);

        return ast;
    }

} // namespace drafter
