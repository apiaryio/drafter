//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "StringUtility.h"

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
        element->set(refract::IElement::Create(metadata.first), refract::IElement::Create(metadata.second));
        element->renderType(refract::IElement::rFull);

        return element;
    }

    refract::IElement* MetadataCollectionToRefract(const snowcrash::MetadataCollection metadata)
    {
        if (metadata.empty()) {
            return NULL;
        }

        refract::ObjectElement* element = new refract::ObjectElement;
        RefractElements content;

        std::transform(metadata.begin(), metadata.end(), std::back_inserter(content), MetadataToRefract);
        element->set(content);
        element->renderType(refract::IElement::rFull);

        return element;
    }

    refract::IElement* CopyToRefract(const std::string& copy)
    {
        refract::IElement* element = refract::IElement::Create(copy);
        element->element("copy");

        return element;
    }

    refract::IElement* ResourceToRefract(const snowcrash::Resource& resource)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements elements;

        element->element("resource");
        element->meta["classes"] = CreateArrayElement("resource");
        element->meta["title"] = refract::IElement::Create(resource.name);

        if (!resource.description.empty()) {
            elements.push_back(CopyToRefract(resource.description));
        }

        element->attributes["href"] = refract::IElement::Create(resource.uriTemplate);
        element->set(elements);

        return element;
    }

    refract::IElement* CategoryToRefract(const snowcrash::Element& element)
    {
        refract::ArrayElement* category = new refract::ArrayElement;
        category->element("category");

        if (element.category == snowcrash::Element::ResourceGroupCategory) {
            category->meta["classes"] = CreateArrayElement("resourceGroup");
            category->meta["title"] = refract::IElement::Create(element.attributes.name);
        }
        else if (element.category == snowcrash::Element::DataStructureGroupCategory) {
            category->meta["classes"] = CreateArrayElement("dataStructures");
        }

        RefractElements elements;
        std::transform(element.content.elements().begin(), element.content.elements().end(),
                       std::back_inserter(elements),
                       ElementToRefract);

        elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        category->set(elements);

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
        RefractElements elements;

        ast->element("category");
        ast->meta["classes"] = CreateArrayElement("api");
        ast->meta["title"] = refract::IElement::Create(blueprint.name);

        if (!blueprint.description.empty()) {
            elements.push_back(CopyToRefract(blueprint.description));
        }

        // FIXME: Correct this according to spec
        if (!blueprint.metadata.empty()) {
            ast->attributes["meta"] = MetadataCollectionToRefract(blueprint.metadata);
        }

        // Append set of elements to content
        const snowcrash::Elements& scElements = blueprint.content.elements();
        std::transform(scElements.begin(), scElements.end(), std::back_inserter(elements), ElementToRefract);

        // Remove NULL elements
        elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        ast->set(elements);

        return ast;
    }

} // namespace drafter
