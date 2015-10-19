//
//  RefractSourceMap.h
//  drafter
//
//  Created by Jiri Kratochvil on 31/07/15.
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#ifndef DRAFTER_REFRACTSOURCEMAP_H
#define DRAFTER_REFRACTSOURCEMAP_H

#include "Serialize.h"

namespace drafter {


    refract::IElement* BytesRangeToRefract(const mdp::BytesRange& bytesRange);

    template<typename T>
    refract::IElement* SourceMapToRefract(const T& sourceMap) 
    {
        refract::ArrayElement* element = new refract::ArrayElement;
        element->element(SerializeKey::SourceMap);

        element->renderType(refract::IElement::rCompactContent);

        refract::ArrayElement* sourceMapElement = new refract::ArrayElement;
        sourceMapElement->element(SerializeKey::SourceMap);

        std::vector<refract::IElement*> ranges;
        std::transform(sourceMap.begin(), sourceMap.end(), std::back_inserter(ranges), BytesRangeToRefract);

        sourceMapElement->set(ranges);

        element->push_back(sourceMapElement);

        return element;
    }

    template<typename T>
    void AttachSourceMap(refract::IElement* element, const T& sectionInfo) 
    {
        if (sectionInfo.hasSourceMap() && !sectionInfo.sourceMap.sourceMap.empty()) {
            element->attributes[SerializeKey::SourceMap] = SourceMapToRefract(sectionInfo.sourceMap.sourceMap);
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

}

#endif // #ifndef DRAFTER_REFRACTSOURCEMAP_H
