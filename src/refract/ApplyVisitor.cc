#include "Element.h"
#include "Visitors.h"

namespace refract {

    ApplyVisitor::~ApplyVisitor() {
        delete apply;
    }

    template<typename T>
    void ApplyVisitor::visit(const T& e) {
        apply->visit(e);
    }

    template<>
    void ApplyVisitor::visit<IElement>(const IElement& e) {
        e.content(*this);
    }
  
    // Explicit instantioning of templates to avoid Linker Error
    template void ApplyVisitor::visit<NullElement>(const NullElement&);
    template void ApplyVisitor::visit<StringElement>(const StringElement&);
    template void ApplyVisitor::visit<NumberElement>(const NumberElement&);
    template void ApplyVisitor::visit<BooleanElement>(const BooleanElement&);
    template void ApplyVisitor::visit<ArrayElement>(const ArrayElement&);
    template void ApplyVisitor::visit<EnumElement>(const EnumElement&);
    template void ApplyVisitor::visit<MemberElement>(const MemberElement&);
    template void ApplyVisitor::visit<ObjectElement>(const ObjectElement&);
    template void ApplyVisitor::visit<ExtendElement>(const ExtendElement&);

}
