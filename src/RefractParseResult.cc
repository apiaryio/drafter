//
//  RefractParseResult.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 25/09/15.
//  Copyright (c) 2015 Apiary. All rights reserved.
//

#include "RefractAPI.h"
#include "RefractParseResult.h"

namespace drafter {

    typedef std::vector<refract::IElement*> RefractElements;

    refract::IElement* SourceMapRowToRefract(const mdp::CharactersRange& sourceMap)
    {
        refract::ArrayElement* element = new refract::ArrayElement;

        element->push_back(refract::IElement::Create((int) sourceMap.location));
        element->push_back(refract::IElement::Create((int) sourceMap.length));

        return element;
    }

    refract::IElement* SourceMapToRefract(const mdp::CharactersRangeSet& sourceMap)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::SourceMap);

        for (mdp::CharactersRangeSet::const_iterator it = sourceMap.begin();
             it != sourceMap.end();
             ++it) {

            content.push_back(SourceMapRowToRefract(*it));
        }

        element->set(content);

        return element;
    }

    refract::IElement* AnnotationToRefract(const snowcrash::SourceAnnotation& annotation, bool error = false)
    {
        refract::IElement* element = refract::IElement::Create(annotation.message);

        element->element(SerializeKey::Annotation);
        element->meta[SerializeKey::Classes] = CreateArrayElement(error ? SerializeKey::Error : SerializeKey::Warning);

        refract::ArrayElement* sourceMap = new refract::ArrayElement;
        sourceMap->push_back(SourceMapToRefract(annotation.location));

        element->attributes[SerializeKey::AnnotationCode] = refract::IElement::Create(annotation.code);
        element->attributes[SerializeKey::SourceMap] = sourceMap;

        return element;
    }

    refract::IElement* ParseResultToRefract(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::ParseResult);

        content.push_back(BlueprintToRefract(blueprint.node));

        element->set(content);

        return element;
    }
}
