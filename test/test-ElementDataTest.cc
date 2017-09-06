#include "catch.hpp"

#include "ElementData.h"

using namespace refract;
using namespace drafter;

TEST_CASE("Check Complex/Primitive data","[ElementData]") {

    REQUIRE(ElementData<NullElement>::IsPrimitive::value == true);

    REQUIRE(ElementData<StringElement>::IsPrimitive::value == true);
    REQUIRE(ElementData<NumberElement>::IsPrimitive::value == true);
    REQUIRE(ElementData<BooleanElement>::IsPrimitive::value == true);

    REQUIRE(ElementData<MemberElement>::IsPrimitive::value == true); // probably invalid, but not used while conversion (maybe static assert?)

    REQUIRE(ElementData<RefElement>::IsPrimitive::value == true);

    REQUIRE(ElementData<ArrayElement>::IsPrimitive::value == false);
    REQUIRE(ElementData<EnumElement>::IsPrimitive::value == false);
    REQUIRE(ElementData<ObjectElement>::IsPrimitive::value == false);

    REQUIRE(ElementData<OptionElement>::IsPrimitive::value == false);
    REQUIRE(ElementData<SelectElement>::IsPrimitive::value == true);

    REQUIRE(ElementData<HolderElement>::IsPrimitive::value == false);

}
