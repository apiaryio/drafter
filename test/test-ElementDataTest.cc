#include "ElementData.h"

using namespace refract;
using namespace drafter;

static_assert(is_primitive<StringElement>, "");
static_assert(is_primitive<NumberElement>, "");
static_assert(is_primitive<BooleanElement>, "");

static_assert(!is_primitive<NullElement>, "");
static_assert(!is_primitive<MemberElement>, "");
static_assert(!is_primitive<RefElement>, "");
static_assert(!is_primitive<ArrayElement>, "");
static_assert(!is_primitive<EnumElement>, "");
static_assert(!is_primitive<ObjectElement>, "");
static_assert(!is_primitive<OptionElement>, "");
static_assert(!is_primitive<SelectElement>, "");
static_assert(!is_primitive<HolderElement>, "");
