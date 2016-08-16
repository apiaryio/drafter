//
//  ConversionContext.cc
//  drafter
//
//  Created by Pavan Kumar Sunkara on 18/08/16.
//  Copyright © 2016 Apiary. All rights reserved.
//

#include "ConversionContext.h"

namespace drafter {

    void ConversionContext::warn(const snowcrash::Warning& warning)
    {
        for (auto& item : warnings) {
            bool equalSourceMap = true;

            // Compare sourcemap
            if (item.location.size() == warning.location.size()) {
                for (size_t i = 0; i < item.location.size(); i++) {
                    if (item.location.at(i).length != warning.location.at(i).length ||
                        item.location.at(i).location != warning.location.at(i).location) {

                        equalSourceMap = false;
                    }
                }
            }
            else {
                equalSourceMap = false;
            }

            if (equalSourceMap && item.code == warning.code && item.message == warning.message) {
                return;
            }
        }

        warnings.push_back(warning);
    }
}