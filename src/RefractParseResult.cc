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

    refract::IElement* AnnotationToRefract(const snowcrash::SourceAnnotation& annotation, bool isError)
    {
        refract::IElement* element = refract::IElement::Create(annotation.message);

        element->element(SerializeKey::Annotation);

        element->meta[SerializeKey::Classes] = CreateArrayElement(isError ? SerializeKey::Error : SerializeKey::Warning);

        element->attributes[SerializeKey::AnnotationCode] = refract::IElement::Create(annotation.code);
        element->attributes[SerializeKey::SourceMap] = SourceMapToRefract(annotation.location);

        return element;
    }

    // Wrapper function because we want to use std::transform over a list with this
    refract::IElement* WarningToRefract(const snowcrash::SourceAnnotation& annotation)
    {
        return AnnotationToRefract(annotation, false);
    }

    refract::IElement* ParseResultToRefract(const snowcrash::ParseResult<snowcrash::Blueprint>& blueprint,
                                            const WrapperOptions& options)
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        RefractElements content;

        element->element(SerializeKey::ParseResult);

        content.push_back(BlueprintToRefract(MakeNodeInfo(blueprint.node, blueprint.sourceMap, options.exportSourceMap)));

        if (blueprint.report.error.code != snowcrash::Error::OK) {
            content.push_back(AnnotationToRefract(blueprint.report.error, true));
        }

        snowcrash::Warnings warnings = blueprint.report.warnings;
        std::transform(warnings.begin(), warnings.end(), std::back_inserter(content), WarningToRefract);

        element->set(content);

        return element;
    }
}
