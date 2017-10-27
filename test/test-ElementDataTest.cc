#include "ElementData.h"

using namespace refract;
using namespace drafter;

static_assert(IsPrimitive<NullElement>::type::value, "NullElement must be primitive");

static_assert(IsPrimitive<StringElement>::type::value, "StringElement must be primitive");
static_assert(IsPrimitive<NumberElement>::type::value, "NumberElement must be primitive");
static_assert(IsPrimitive<BooleanElement>::type::value, "BooleanElement must be primitive");

static_assert(IsPrimitive<MemberElement>::type::value,
    "MemeberElement should be primitive"); // probably invalid, but not used while conversion

static_assert(IsPrimitive<RefElement>::type::value, "RefElement should be primitive");

static_assert(!IsPrimitive<ArrayElement>::type::value, "ArrayElement must not be primitive");
static_assert(!IsPrimitive<EnumElement>::type::value, "EnumElement must not be primitive");
static_assert(!IsPrimitive<ObjectElement>::type::value, "ObjectElement must not be primitive");

static_assert(!IsPrimitive<OptionElement>::type::value, "OptionElement must not be primitive");
static_assert(IsPrimitive<SelectElement>::type::value, "SelectElement must be primitive");

static_assert(!IsPrimitive<HolderElement>::type::value, "HolderElement must not be primitive");
