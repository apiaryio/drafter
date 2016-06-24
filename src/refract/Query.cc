#include "Query.h"
#include "Element.h"

namespace refract {

    namespace query {

            bool Element::operator()(const IElement& e) {
                return e.element() == name;
            }
    }

}
