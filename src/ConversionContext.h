//
//  ConversionContext.h
//  drafter
//
//  Created by Jiri Kratochvil on 06-04-2016
//  Copyright (c) 2016 Apiary Inc. All rights reserved.
//
#ifndef DRAFTER_CONVERSIONCONTEXT_H
#define DRAFTER_CONVERSIONCONTEXT_H

#include <boost/container/vector.hpp>

#include "refract/Registry.h"
#include "SourceMapUtils.h"
#include "options.h"

namespace snowcrash
{
    struct SourceAnnotation;
}

namespace drafter
{
    class ConversionContext
    {
    public:
        using Warnings = boost::container::vector<snowcrash::SourceAnnotation>;

    private:
        const NewLinesIndex newline_indices_;
        const bool expand_mson_;
        const drafter_parse_options* const options_;

        refract::Registry registry_;
        Warnings warnings_;

    public:
        explicit ConversionContext( //
            const char*,
            const drafter_parse_options* opts = nullptr,
            bool expandMson = false // TODO avoid, only used in unit tests
            ) noexcept;

        const NewLinesIndex& newlineIndices() const noexcept;

        bool expandMson() const noexcept;

        refract::Registry& typeRegistry() noexcept;
        const refract::Registry& typeRegistry() const noexcept;

        const Warnings& warnings() const noexcept;
        void warn(const snowcrash::Warning& warning);

        const drafter_parse_options* options() const noexcept;
    };
}
#endif
