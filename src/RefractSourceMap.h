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


    refract::IElement* SourceMapToRefract(const mdp::CharactersRangeSet& sourceMap);

    template<typename T>
    void AttachSourceMap(refract::IElement* element, const T& nodeInfo)
    {
        if (nodeInfo.hasSourceMap() && !nodeInfo.sourceMap->sourceMap.empty()) {
            element->attributes[SerializeKey::SourceMap] = SourceMapToRefract(nodeInfo.sourceMap->sourceMap);
            element->renderType(refract::IElement::rFull);
        }
    }

    template<typename T>
    refract::IElement* PrimitiveToRefract(const NodeInfo<T>& primitive)
    {
        typedef typename refract::ElementTypeSelector<T>::ElementType ElementType;

        ElementType* element = refract::IElement::Create(*primitive.node);

        AttachSourceMap(element, primitive);

        return element;
    }

    template<typename T>
    refract::IElement* LiteralToRefract(const NodeInfo<std::string>& literal)
    {
        refract::IElement* element = refract::IElement::Create(LiteralTo<T>(*literal.node));

        AttachSourceMap(element, literal);

        return element;
    }

}

#endif // #ifndef DRAFTER_REFRACTSOURCEMAP_H
