//
//  ConversionContext.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/16.
//  Copyright (c) 2016 Apiary. All rights reserved.
//

#include "ConversionContext.h"

#include "snowcrash.h"

using namespace drafter;

ConversionContext::ConversionContext(const char* src, const drafter_parse_options* opts, bool expandMson) noexcept
    : newline_indices_(GetLinesEndIndex(src)),
      expand_mson_{ expandMson },
      options_{ opts },
      registry_{},
      warnings_{}
{
}

refract::Registry& ConversionContext::typeRegistry() noexcept
{
    return registry_;
}

const refract::Registry& ConversionContext::typeRegistry() const noexcept
{
    return registry_;
}

const NewLinesIndex& ConversionContext::newlineIndices() const noexcept
{
    return newline_indices_;
}

bool ConversionContext::expandMson() const noexcept
{
    return expand_mson_;
}

void ConversionContext::warn(const snowcrash::Warning& warning)
{
    for (auto& item : warnings_) {
        bool equalSourceMap = true;

        // Compare sourcemap
        if (item.location.size() == warning.location.size()) {
            for (size_t i = 0; i < item.location.size(); i++) {
                if (item.location.at(i).length != warning.location.at(i).length
                    || item.location.at(i).location != warning.location.at(i).location) {

                    equalSourceMap = false;
                }
            }
        } else {
            equalSourceMap = false;
        }

        if (equalSourceMap && item.code == warning.code && item.message == warning.message) {
            return;
        }
    }

    warnings_.push_back(warning);
}

const ConversionContext::Warnings& ConversionContext::warnings() const noexcept
{
    return warnings_;
}

const drafter_parse_options* ConversionContext::options() const noexcept
{
    return options_;
}
