#include "catch.hpp"

#include "ElementData.h"

using namespace refract;
using namespace drafter;

TEST_CASE("Check Complex/Primitive data","[ElementData]") {

    REQUIRE(ElementData<NullElement>::IsPrimitive::value);

    REQUIRE(ElementData<StringElement>::IsPrimitive::value);
    REQUIRE(ElementData<NumberElement>::IsPrimitive::value);
    REQUIRE(ElementData<BooleanElement>::IsPrimitive::value);

    REQUIRE(ElementData<MemberElement>::IsPrimitive::value); // probably invalid, but not used while conversion (maybe static assert?)

    REQUIRE(ElementData<RefElement>::IsPrimitive::value);

    REQUIRE_FALSE(ElementData<ArrayElement>::IsPrimitive::value);
    REQUIRE_FALSE(ElementData<EnumElement>::IsPrimitive::value);
    REQUIRE_FALSE(ElementData<ObjectElement>::IsPrimitive::value);

    REQUIRE_FALSE(ElementData<OptionElement>::IsPrimitive::value);
    REQUIRE(ElementData<SelectElement>::IsPrimitive::value);

    REQUIRE_FALSE(ElementData<HolderElement>::IsPrimitive::value);

}
