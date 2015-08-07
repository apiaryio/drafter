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

    typedef std::vector<refract::IElement*> RefractElements;

    // Forward Declarations
    refract::IElement* ElementToRefract(const snowcrash::Element& element);

    template <typename T>
    static bool IsNull(const T* ptr)
    {
        return ptr == NULL;
    }

    refract::IElement* MetadataToRefract(const snowcrash::Metadata metadata)
    {
        refract::MemberElement* element = new refract::MemberElement;

        element->meta["class"] = refract::ArrayElement::Create("user");
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

    refract::IElement* ResourceToRefract(const snowcrash::Element& element)
    {
        refract::ArrayElement* resource = new refract::ArrayElement;
        resource->element("resource");

        return resource;
    }

    refract::IElement* CopyToRefract(const snowcrash::Element& element)
    {
        refract::IElement* text = refract::IElement::Create(element.content.copy);
        text->element("copy");

        return text;
    }

    refract::IElement* CategoryToRefract(const snowcrash::Element& element)
    {
        refract::ArrayElement* category = new refract::ArrayElement;
        category->element("category");

        if (element.category == snowcrash::Element::ResourceGroupCategory) {
            category->meta["class"] = refract::ArrayElement::Create("resourceGroup");
            category->meta["title"] = refract::IElement::Create(element.attributes.name);
        }
        else if (element.category == snowcrash::Element::DataStructureGroupCategory) {
            category->meta["class"] = refract::ArrayElement::Create("dataStructures");
        }

        RefractElements elements;
        std::transform(element.content.elements().begin(), element.content.elements().end(), std::back_inserter(elements), ElementToRefract);

        elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        category->set(elements);

        return category;
    }

    refract::IElement* ElementToRefract(const snowcrash::Element& element)
    {
        switch (element.element) {
            case snowcrash::Element::ResourceElement:
                return ResourceToRefract(element);
            case snowcrash::Element::DataStructureElement:
                return DataStructureToRefract(element.content.dataStructure);
            case snowcrash::Element::CopyElement:
                return CopyToRefract(element);
            case snowcrash::Element::CategoryElement:
                return CategoryToRefract(element);
            default:
                throw std::runtime_error("Not Implemented - Unhandled type of Element");
        }
    }

    refract::IElement* BlueprintToRefract(const snowcrash::Blueprint& blueprint)
    {
        refract::ArrayElement* ast = new refract::ArrayElement;
        ast->element("category");

        ast->meta["class"] = refract::ArrayElement::Create("api");
        ast->meta["title"] = refract::IElement::Create(blueprint.name);

        if (!blueprint.description.empty()) {
            ast->meta["description"] = refract::IElement::Create(blueprint.description);
        }

        refract::IElement* metadata = MetadataCollectionToRefract(blueprint.metadata);

        if (metadata) {
            ast->attributes["meta"] = metadata;
        }

        RefractElements elements;
        const snowcrash::Elements& scElements = blueprint.content.elements();

        // Append set of elements to content
        std::transform(scElements.begin(), scElements.end(), std::back_inserter(elements), ElementToRefract);

        // Remove NULL elements
        elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());
        ast->set(elements);

        return ast;
    }

} // namespace drafter
