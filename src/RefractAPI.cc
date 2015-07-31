//
//  RefractAPI.cc
//  drafter
//
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "StringUtility.h"

#include "RefractAPI.h"
#include "refract/Element.h"
#include "refract/Registry.h"
#include "refract/Visitors.h"


refract::IElement* BlueprintToRefract(const snowcrash::Blueprint& blueprint) {
    return NULL;
}

sos::Object drafter::SerializeToRefract(const snowcrash::Blueprint& blueprint) {
    return sos::Object();
}

