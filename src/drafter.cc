//
//  drafter.cc
//  drafter
//
//  Created by Jiri Kratochvil on 2015-03-05
//  Copyright (c) 2015 Apiary Inc. All rights reserved.
//

#include "drafter.h"

namespace drafter {

    /**
     * For now just redirect to snowcrash::parse()
     */
    int ParseBlueprint(const mdp::ByteBuffer& source,
                       snowcrash::BlueprintParserOptions options,
                       const snowcrash::ParseResultRef<snowcrash::Blueprint>& out)
    {
        return snowcrash::parse(source, options, out);
    }
}
