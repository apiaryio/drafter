//
//  RefractAPI.cc
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include <iterator>

#include "SourceAnnotation.h"
#include "RefractDataStructure.h"
#include "RefractAPI.h"
#include "Render.h"

namespace drafter {

    // Forward Declarations
    refract::IElement* _ElementToRefract(const SectionInfo<snowcrash::Element>& element);

    namespace {

        typedef std::vector<refract::IElement*> RefractElements;
        typedef std::vector<const snowcrash::DataStructure*> DataStructures;

        void FindNamedTypes(const snowcrash::Elements& elements, DataStructures& found)
        {
            for (snowcrash::Elements::const_iterator i = elements.begin() ; i != elements.end() ; ++i) {

                if (i->element == snowcrash::Element::DataStructureElement) {
                    found.push_back(&(i->content.dataStructure));
                }
                else if (!i->content.resource.attributes.empty()) {
                    found.push_back(&i->content.resource.attributes);
                }
                else if (i->element == snowcrash::Element::CategoryElement) {
                    FindNamedTypes(i->content.elements(), found);
                }
            }
        }

        refract::ArrayElement* CreateArrayElement(refract::IElement* value, bool rFull = false)
        {
            refract::ArrayElement* array = new refract::ArrayElement;

            if (rFull) {
                value->renderType(refract::IElement::rFull);
            }

            array->push_back(value);
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

        template<typename T, typename C, typename F>
        refract::IElement* CollectionToRefract(const SectionInfo<C>& collection, const F& transformFunctor, const std::string& key = std::string(), const refract::IElement::renderFlags renderType = refract::IElement::rCompact)
        {
            T* element = new T;
            RefractElements content;

            if (!key.empty()) {
                element->element(key);
            }

            SectionInfoCollection<C> sectionInfoCollection(collection.section, collection.sourceMap);

            std::transform(sectionInfoCollection.sections.begin(), sectionInfoCollection.sections.end(), std::back_inserter(content), transformFunctor);

            element->set(content);

            element->renderType(renderType);

            return element;
        }

    }

    refract::IElement* BytesRangeToRefract(const mdp::BytesRange& bytesRange)
    {
        refract::ArrayElement* range = new refract::ArrayElement;

        range->push_back(refract::IElement::Create(bytesRange.location));
        range->push_back(refract::IElement::Create(bytesRange.length));

        return range;
    }

    template<typename T>
    refract::IElement* SourceMapToRefract(const T& sourceMap) 
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        element->element("sourceMap");
        element->renderType(refract::IElement::rFull);

        refract::ArrayElement* sourceMapElement = new refract::ArrayElement;
        sourceMapElement->element("sourceMap");
        element->renderType(refract::IElement::rCompact);

        RefractElements ranges;
        std::transform(sourceMap.begin(), sourceMap.end(), std::back_inserter(ranges), BytesRangeToRefract);

        sourceMapElement->set(ranges);
        element->push_back(sourceMapElement);

        return element;
    }

    template<typename T>
    void AttachSourceMap(refract::IElement* element, const T& sectionInfo) 
    {
        if (sectionInfo.hasSourceMap() && !sectionInfo.sourceMap.sourceMap.empty()) {
            element->attributes["sourceMap"] = SourceMapToRefract(sectionInfo.sourceMap.sourceMap);
            // FIXME: how to render nonrefract elments? eg. "title"
            // solution -> use Expanded form
            element->renderType(refract::IElement::rFull);
        }
    }

    template<typename T>
    refract::IElement* PrimitiveToRefract(const SectionInfo<T>& sectionInfo)
    {
        typedef typename refract::ElementTypeSelector<T>::ElementType ElementType;

        ElementType* element = refract::IElement::Create(sectionInfo.section);
        AttachSourceMap(element, sectionInfo);

        return element;
    }

    template<typename T>
    refract::IElement* LiteralToRefract(const SectionInfo<std::string>& sectionInfo)
    {
        refract::IElement* element = refract::IElement::Create(LiteralTo<T>(sectionInfo.section));
        AttachSourceMap(element, sectionInfo);

        return element;
    }

    template <typename T>
    refract::ArrayElement* CreateArrayElement(const T& content, bool rFull)
    {
        refract::ArrayElement* array = new refract::ArrayElement;
        refract::IElement* value = refract::IElement::Create(content);

        if (rFull) {
            value->renderType(refract::IElement::rFull);
        }

        array->push_back(value);
        return array;
    }

    void RegisterNamedTypes(const snowcrash::Elements& elements)
    {
        DataStructures found;
        FindNamedTypes(elements, found);

        for (DataStructures::const_iterator i = found.begin(); i != found.end(); ++i) {

            if (!(*i)->name.symbol.literal.empty()) {
                refract::IElement* element = _MSONToRefract(MakeSectionInfoWithoutSourceMap(*(*i)));
                GetNamedTypesRegistry().add(element);
            }
        }
    }

    refract::IElement* DataStructureToRefract(const snowcrash::DataStructure& dataStructure, bool expand)
    {
       refract::IElement* msonElement = _MSONToRefract(MakeSectionInfoWithoutSourceMap(dataStructure));

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

    refract::IElement* _DataStructureToRefract(const SectionInfo<snowcrash::DataStructure>& dataStructure, bool expand)
    {
        refract::IElement* msonElement = _MSONToRefract(dataStructure);

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

    refract::IElement* _MetadataToRefract(const SectionInfo<snowcrash::Metadata>& metadata)
    {
        refract::MemberElement* element = new refract::MemberElement;

        refract::ArrayElement* classes = CreateArrayElement(SerializeKey::User);
        classes->renderType(refract::IElement::rCompact);

        element->meta[SerializeKey::Classes] = classes;
        element->set(refract::IElement::Create(metadata.section.first), refract::IElement::Create(metadata.section.second));
        element->renderType(refract::IElement::rFull);

        AttachSourceMap(element, metadata);

        return element;
    }

    refract::IElement* _CopyToRefract(const SectionInfo<std::string>& copy)
    {
        if (copy.section.empty()) {
            return NULL;
        }

        refract::IElement* element = PrimitiveToRefract(copy);
        element->element(SerializeKey::Copy);

        return element;
    }

    template<typename T>
    refract::IElement* _ParameterValuesToRefract(const SectionInfo<snowcrash::Parameter>& parameter)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::Enum);

        // FIXME: nearly duplicit code in _ExtractParameter()
        // Add sample value
        if (!parameter.section.exampleValue.empty()) {
            refract::ArrayElement* samples = new refract::ArrayElement;
            // FIXME: sourcemap of exampleValue does is not equal to
            samples->push_back(CreateArrayElement(LiteralToRefract<T>(MAKE_SECTION_INFO(parameter, exampleValue)), true));
            element->attributes[SerializeKey::Samples] = samples;
        }

        // Add default value
        if (!parameter.section.defaultValue.empty()) {
            element->attributes[SerializeKey::Default] = CreateArrayElement(LiteralToRefract<T>(MAKE_SECTION_INFO(parameter, defaultValue)), true);
        }

        SectionInfoCollection<snowcrash::Values> values(parameter.section.values, parameter.sourceMap.values);
        std::transform(values.sections.begin(), values.sections.end(), std::back_inserter(content), LiteralToRefract<T>);

        element->set(content);

        return element;
    }

    template<typename T>
    refract::IElement* _ExtractParameter(const SectionInfo<snowcrash::Parameter>& parameter)
    {
        refract::IElement* element = NULL;

        if (parameter.section.values.empty()) {
            element = refract::IElement::Create(LiteralTo<T>(parameter.section.exampleValue));
            AttachSourceMap(element, MAKE_SECTION_INFO(parameter, exampleValue));

            if (!parameter.section.defaultValue.empty()) {
                refract::IElement* defaultElement = refract::IElement::Create(LiteralTo<T>(parameter.section.defaultValue));
                AttachSourceMap(defaultElement, MAKE_SECTION_INFO(parameter, defaultValue));
                element->attributes[SerializeKey::Default] = defaultElement;
            }
        }
        else {
            element = _ParameterValuesToRefract<T>(parameter);
        }

        return element;
    }

    refract::IElement* _ParameterToRefract(const SectionInfo<snowcrash::Parameter>& parameter)
    {
        refract::MemberElement* element = new refract::MemberElement;

        refract::IElement *value = NULL;

        // Parameter type, exampleValue, defaultValue, values
        if (parameter.section.type == "boolean") {
            value = _ExtractParameter<bool>(parameter);
        }
        else if (parameter.section.type == "number") {
            value = _ExtractParameter<double>(parameter);
        }
        else {
            value = _ExtractParameter<std::string>(parameter);
        }

        element->set(PrimitiveToRefract(MAKE_SECTION_INFO(parameter, name)), value);

        // Description
        if (!parameter.section.description.empty()) {
            element->meta[SerializeKey::Description] = PrimitiveToRefract(MAKE_SECTION_INFO(parameter, description));
        }

        // Parameter use
        if (parameter.section.use == snowcrash::RequiredParameterUse || parameter.section.use == snowcrash::OptionalParameterUse) {
            refract::ArrayElement* typeAttributes = new refract::ArrayElement;

            typeAttributes->push_back(refract::IElement::Create(parameter.section.use == snowcrash::RequiredParameterUse ? SerializeKey::Required : SerializeKey::Optional));
            element->attributes[SerializeKey::TypeAttributes] = typeAttributes;
        }

        return element;
    }

    refract::IElement* _ParametersToRefract(const SectionInfo<snowcrash::Parameters>& parameters)
    {
        return CollectionToRefract<refract::ObjectElement>(parameters, _ParameterToRefract, SerializeKey::HrefVariables, refract::IElement::rFull);
    }

    refract::IElement* _HeaderToRefract(const SectionInfo<snowcrash::Header>& header)
    {
        refract::MemberElement* element = new refract::MemberElement;

        element->set(refract::IElement::Create(header.section.first), refract::IElement::Create(header.section.second));

        AttachSourceMap(element, header);

        return element;
    }

    refract::IElement* AssetToRefract(const snowcrash::Asset& asset, const std::string& contentType, bool messageBody = true)
    {
        if (asset.empty()) {
            return NULL;
        }

        refract::IElement* element = refract::IElement::Create(asset);

        element->element(SerializeKey::Asset);
        element->meta[SerializeKey::Classes] = refract::ArrayElement::Create(messageBody ? SerializeKey::MessageBody : SerializeKey::MessageSchema);

        if (!contentType.empty()) {
            element->attributes[SerializeKey::ContentType] = refract::IElement::Create(contentType);
        }

        return element;
    }

    refract::IElement* _PayloadToRefract(const SectionInfo<snowcrash::Payload>& payload, const SectionInfo<snowcrash::Action>& action)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        // Use HTTP method to recognize if request or response
        if (action.isNull() || action.section.method.empty()) {
            element->element(SerializeKey::HTTPResponse);

            if (!payload.isNull()) {
                element->attributes[SerializeKey::StatusCode] = PrimitiveToRefract(MAKE_SECTION_INFO(payload, name));
            }
        }
        else {
            element->element(SerializeKey::HTTPRequest);
            element->attributes[SerializeKey::Method] = PrimitiveToRefract(MAKE_SECTION_INFO(action, method));

            if (!payload.isNull()) {
                element->attributes[SerializeKey::Title] = PrimitiveToRefract(MAKE_SECTION_INFO(payload, name));
            }
        }

        // If no payload, return immediately
        if (payload.isNull()) {
            element->set(content);
            return element;
        }

        if (!payload.section.headers.empty()) {
            element->attributes[SerializeKey::Headers] = CollectionToRefract<refract::ArrayElement>(MAKE_SECTION_INFO(payload, headers),
                                                                                                    _HeaderToRefract,
                                                                                                    SerializeKey::HTTPHeaders,
                                                                                                    refract::IElement::rFull);
        }

        // Render using boutique
        // FIXME: has asset sourcemap?
        snowcrash::Asset payloadBody = renderPayloadBody(payload.section, action.isNull() ? NULL : &action.section, GetNamedTypesRegistry());
        snowcrash::Asset payloadSchema = renderPayloadSchema(payload.section);

        content.push_back(_CopyToRefract(MAKE_SECTION_INFO(payload, description)));
        content.push_back(_DataStructureToRefract(MAKE_SECTION_INFO(payload, attributes)));

        // Get content type
        std::string contentType = getContentTypeFromHeaders(payload.section.headers);

        // Assets
        // FIXME: SourceMap
        content.push_back(AssetToRefract(payloadBody, contentType));
        content.push_back(AssetToRefract(payloadSchema, contentType, false));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* _TransactionToRefract(const SectionInfo<snowcrash::TransactionExample>& transaction,
                                            const SectionInfo<snowcrash::Action>& action,
                                            const SectionInfo<snowcrash::Request>& request,
                                            const SectionInfo<snowcrash::Response>& response)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::HTTPTransaction);
        content.push_back(_CopyToRefract(MAKE_SECTION_INFO(transaction, description)));

        content.push_back(_PayloadToRefract(request, action));
        content.push_back(_PayloadToRefract(response, SectionInfo<snowcrash::Action>()));

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* _ActionToRefract(const SectionInfo<snowcrash::Action>& action)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::Transition);
        element->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_SECTION_INFO(action, name));

        if (!action.section.relation.str.empty()) {
            // FIXME: add SourceMap
            element->attributes[SerializeKey::Relation] = refract::IElement::Create(action.section.relation.str);
        }

        if (!action.section.uriTemplate.empty()) {
            // FIXME: This doues not chnge rendering, do we have exammple with uriTemplate?
            element->attributes[SerializeKey::Href] = PrimitiveToRefract(MAKE_SECTION_INFO(action, uriTemplate));
        }

        if (!action.section.parameters.empty()) {
            element->attributes[SerializeKey::HrefVariables] = _ParametersToRefract(MAKE_SECTION_INFO(action, parameters));
        }

        if (!action.section.attributes.empty()) {
            refract::IElement* dataStructure = _DataStructureToRefract(MAKE_SECTION_INFO(action, attributes));
            dataStructure->renderType(refract::IElement::rFull);
            element->attributes[SerializeKey::Data] = dataStructure;
        }

        content.push_back(_CopyToRefract(MAKE_SECTION_INFO(action, description)));

        typedef SectionInfoCollection<snowcrash::TransactionExamples> ExamplesType;
        ExamplesType examples(action.section.examples, action.sourceMap.examples);

        for (ExamplesType::ConstIterarator it = examples.sections.begin();
             it != examples.sections.end();
             ++it) {

            // When there are only responses
            if (it->section.requests.empty() && !it->section.responses.empty()) {

                typedef SectionInfoCollection<snowcrash::Responses> ResponsesType;
                ResponsesType responses(it->section.responses, it->sourceMap.responses);

                for (ResponsesType::ConstIterarator resIt = responses.sections.begin() ;
                     resIt != responses.sections.end();
                     ++resIt) {

                    content.push_back(_TransactionToRefract(*it, action, SectionInfo<snowcrash::Request>(), *resIt));
                }
            }

            // When there are only requests or both responses and requests
            typedef SectionInfoCollection<snowcrash::Requests> RequestsType;
            RequestsType requests(it->section.requests, it->sourceMap.requests);

            for (RequestsType::ConstIterarator reqIt = requests.sections.begin();
                 reqIt != requests.sections.end();
                 ++reqIt) {

                if (it->section.responses.empty()) {
                    content.push_back(_TransactionToRefract(*it, action, *reqIt, SectionInfo<snowcrash::Response>()));
                }

                typedef SectionInfoCollection<snowcrash::Responses> ResponsesType;
                ResponsesType responses(it->section.responses, it->sourceMap.responses);

                for (ResponsesType::ConstIterarator resIt = responses.sections.begin();
                     resIt != responses.sections.end();
                     ++resIt) {

                    content.push_back(_TransactionToRefract(*it, action, *reqIt, *resIt));
                }
            }
        }

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* _ResourceToRefract(const SectionInfo<snowcrash::Resource>& resource)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::Resource);

        element->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_SECTION_INFO(resource, name));

        element->attributes[SerializeKey::Href] = PrimitiveToRefract(MAKE_SECTION_INFO(resource, uriTemplate));

        if (!resource.section.parameters.empty()) {
            element->attributes[SerializeKey::HrefVariables] = _ParametersToRefract(MAKE_SECTION_INFO(resource, parameters));
        }

        content.push_back(_CopyToRefract(MAKE_SECTION_INFO(resource, description)));
        content.push_back(_DataStructureToRefract(MAKE_SECTION_INFO(resource, attributes)));

        SectionInfoCollection<snowcrash::Actions> actions(resource.section.actions, resource.sourceMap.actions);
        std::transform(actions.sections.begin(), actions.sections.end(), std::back_inserter(content), _ActionToRefract);

        RemoveEmptyElements(content);
        element->set(content);

        return element;
    }

    refract::IElement* _CategoryToRefract(const SectionInfo<snowcrash::Element>& element)
    {
        refract::ArrayElement* category = new refract::ArrayElement;
        RefractElements content;

        category->element(SerializeKey::Category);

        if (element.section.category == snowcrash::Element::ResourceGroupCategory) {
            category->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::ResourceGroup);
            category->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_SECTION_INFO(element, attributes.name));
        }
        else if (element.section.category == snowcrash::Element::DataStructureGroupCategory) {
            category->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::DataStructures);
        }

        if (!element.section.content.elements().empty()) {
            const snowcrash::SourceMap<snowcrash::Elements>& sourceMap = element.sourceMap.content.elements().collection.empty()
                ? SectionInfo<snowcrash::Elements>::NullSourceMap()
                : element.sourceMap.content.elements();

            SectionInfoCollection<snowcrash::Elements> elements(element.section.content.elements(), sourceMap);
            std::transform(elements.sections.begin(), elements.sections.end(), std::back_inserter(content), _ElementToRefract);
        }

        RemoveEmptyElements(content);
        category->set(content);

        return category;
    }

    refract::IElement* _ElementToRefract(const SectionInfo<snowcrash::Element>& element) 
    {
        switch (element.section.element) {
            case snowcrash::Element::ResourceElement:
                return _ResourceToRefract(MAKE_SECTION_INFO(element, content.resource));
            case snowcrash::Element::DataStructureElement:
                return _DataStructureToRefract(MAKE_SECTION_INFO(element, content.dataStructure));
            case snowcrash::Element::CopyElement:
                return _CopyToRefract(MAKE_SECTION_INFO(element, content.copy));
            case snowcrash::Element::CategoryElement:
                return _CategoryToRefract(element);
            default:
                throw snowcrash::Error("unknown type of api description element", snowcrash::ApplicationError);
        }
    }

    refract::IElement* BlueprintToRefract(const SectionInfo<snowcrash::Blueprint>& blueprint)
    {
        refract::ArrayElement* ast = new refract::ArrayElement;
        RefractElements content;

        ast->element(SerializeKey::Category);
        ast->meta[SerializeKey::Classes] = CreateArrayElement(SerializeKey::API);

        ast->meta[SerializeKey::Title] = PrimitiveToRefract(MAKE_SECTION_INFO(blueprint, name));

        content.push_back(_CopyToRefract(MAKE_SECTION_INFO(blueprint, description)));

        if (!blueprint.section.metadata.empty()) {
            ast->attributes[SerializeKey::Meta] = CollectionToRefract<refract::ArrayElement>(MAKE_SECTION_INFO(blueprint, metadata), _MetadataToRefract);
        }

        SectionInfoCollection<snowcrash::Elements> elements(blueprint.section.content.elements(), blueprint.sourceMap.content.elements());
        std::transform(elements.sections.begin(), elements.sections.end(), std::back_inserter(content), _ElementToRefract);

        RemoveEmptyElements(content);
        ast->set(content);

        return ast;
    }

} // namespace drafter
