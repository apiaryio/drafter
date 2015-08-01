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

typedef std::vector<refract::IElement*> RefractElements;

template <typename T>
static bool IsNull(const T* ptr)
{
    return ptr == NULL;
}

refract::IElement* MetadataToRefract(const snowcrash::Metadata metadata)
{
    refract::MemberElement* element = new refract::MemberElement;
    element->meta["class"] = refract::IElement::Create("user");

    element->set(refract::IElement::Create(metadata.first),
                 refract::IElement::Create(metadata.second));

    element->renderType(refract::IElement::rFull);

    return element;
}

refract::IElement* MetadataCollectionToRefract(const snowcrash::MetadataCollection metadata)
{
    if(metadata.empty()) {
        return NULL;
    }

    refract::ObjectElement* element = new refract::ObjectElement;
    RefractElements content;

    std::transform(metadata.begin(), metadata.end(),
                   std::back_inserter(content),
                   MetadataToRefract);

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

refract::IElement* ElementToRefract(const snowcrash::Element& element)
{
    switch(element.element) {
        case snowcrash::Element::ResourceElement :
            return ResourceToRefract(element);
        case snowcrash::Element::CopyElement :
            return CopyToRefract(element);
        default:
            throw std::runtime_error("Not Implement - Unhandled type of Element");
    }
}

// FIXME: remove duplicity
static bool IsElementResourceGroup(const snowcrash::Element& element)
{
    return element.element == snowcrash::Element::CategoryElement && element.category == snowcrash::Element::ResourceGroupCategory;
}

refract::IElement* ResourceGroupToRefract(const snowcrash::Element& resourceGroup)
{
    if (!IsElementResourceGroup(resourceGroup)) {
        return NULL;
    }

    refract::ArrayElement* group = new refract::ArrayElement;
    group->element("category");
    group->meta["class"] = refract::IElement::Create("resourceGroup");
    group->meta["title"] = refract::IElement::Create(resourceGroup.attributes.name);

    RefractElements elements;
    std::transform(resourceGroup.content.elements().begin(), resourceGroup.content.elements().end(),
                   std::back_inserter(elements),
                   ElementToRefract);

    elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());

    group->set(elements);

    return group;
}

refract::IElement* BlueprintToRefract(const snowcrash::Blueprint& blueprint)
{
    refract::ArrayElement* ast = new refract::ArrayElement;
    ast->element("category");
    ast->meta["class"] = refract::IElement::Create("api");
    ast->meta["title"] = refract::IElement::Create(blueprint.name);
    ast->meta["description"] = refract::IElement::Create(blueprint.description);

    refract::IElement* metadata = MetadataCollectionToRefract(blueprint.metadata);
    if (metadata) {
        ast->attributes["meta"] = metadata;
    }

    // FIXME: there is starting point to prepare Refract AST
    // simple start at sos::Object drafter::WrapBlueprint(const Blueprint& blueprint)
    // and convert all functions into Refract :)
    //
    // do not forgot add test cases into test/test-RefractAstTest.cc
    // for inspiration you can look into test/test-RefractMsonTest.cc

    RefractElements elements;
    const snowcrash::Elements& scElements = blueprint.content.elements();


    // append Resource groups to content
    std::transform(scElements.begin(), scElements.end(), std::back_inserter(elements), ResourceGroupToRefract);

    // append another set of element to content (in same way as happen in WrapBlueprint())
    std::transform(scElements.begin(), scElements.end(), std::back_inserter(elements), ElementToRefract);

    // IDEA: maybe remove NULL from (Array|Object) while add directly in refractlib
    // we need no remove them everywhere "by hand"
    elements.erase(std::remove_if(elements.begin(), elements.end(), IsNull<refract::IElement>), elements.end());

    ast->set(elements);

    return ast;
}

sos::Object drafter::SerializeToRefract(const snowcrash::Blueprint& blueprint)
{

    refract::IElement* ast = BlueprintToRefract(blueprint);

    if (!ast) {
        return sos::Object();
    }

    refract::SerializeVisitor serializer;
    serializer.visit(*ast);

    delete ast;

    return serializer.get();
}

