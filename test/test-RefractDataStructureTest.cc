#include "draftertest.h"

using namespace draftertest;

static drafter::WrapperOptions MSONTestOptions(drafter::NormalASTType, false, true);

static sos::Object Wrapper(snowcrash::ParseResult<snowcrash::Blueprint>& blueprint, const drafter::WrapperOptions& options) {
    return drafter::WrapBlueprint(blueprint, options.expandMSON);
}

#define TEST_MSON(file, mustBeOk) TEST_DRAFTER("Testing MSON serialization for", "mson", file, "ast", &Wrapper, MSONTestOptions, mustBeOk)
#define TEST_MSON_SUCCESS(file) TEST_MSON(file, true)

TEST_MSON_SUCCESS("primitives");
TEST_MSON_SUCCESS("inheritance");
TEST_MSON_SUCCESS("typed-array");
TEST_MSON_SUCCESS("typed-object");
TEST_MSON_SUCCESS("nontyped-object");
TEST_MSON_SUCCESS("enum");
TEST_MSON_SUCCESS("oneof");
TEST_MSON_SUCCESS("mixin");
TEST_MSON("mixin-nonexistent", false); // can fail, nonvalid blueprint
TEST_MSON_SUCCESS("string-sample");
TEST_MSON_SUCCESS("typed-array-sample");
TEST_MSON_SUCCESS("group");
TEST_MSON_SUCCESS("empty-sample");
TEST_MSON_SUCCESS("inner-inheritance");
TEST_MSON_SUCCESS("oneof-sample");
TEST_MSON_SUCCESS("multiline-description");
TEST_MSON_SUCCESS("primitive-variables");
TEST_MSON_SUCCESS("named-with-types");
TEST_MSON_SUCCESS("nontyped-array");
TEST_MSON_SUCCESS("number-wrong-value");
TEST_MSON_SUCCESS("enum-sample");
TEST_MSON("primitive-with-members", false); // can fail, nonvalid blueprint
TEST_MSON_SUCCESS("nontyped-array-sample");
TEST_MSON_SUCCESS("resource-anonymous");
TEST_MSON_SUCCESS("resource-nested-inheritance");
TEST_MSON_SUCCESS("resource-nested-mixin");
TEST_MSON_SUCCESS("resource-unresolved-reference");
TEST_MSON_SUCCESS("resource-resolve-basetype");
TEST_MSON_SUCCESS("resource-primitive-mixin");
TEST_MSON_SUCCESS("array-typed-content");
TEST_MSON_SUCCESS("resource-nested-member");
TEST_MSON_SUCCESS("array-sample");
TEST_MSON_SUCCESS("object-sample");
TEST_MSON_SUCCESS("enum-members-description");
TEST_MSON_SUCCESS("array-reference");
TEST_MSON_SUCCESS("reference-override");
TEST_MSON_SUCCESS("enum-variants");
TEST_MSON_SUCCESS("inheritance-primitive");
TEST_MSON_SUCCESS("regression-207");

TEST_REFRACT("mson", "variable-property-name");

#undef TEST_MSON_SUCCESS
#undef TEST_MSON
