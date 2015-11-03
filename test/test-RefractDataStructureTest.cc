#include "draftertest.h"

using namespace draftertest;

static drafter::WrapperOptions MSONTestOptions(drafter::NormalASTType, true, false);

#define TEST_MSON_(file, mustBeOk) TEST_DRAFTER("Testing MSON serialization for", "mson", file, "ast", &drafter::WrapBlueprint, MSONTestOptions, mustBeOk)

#define TEST_MSON(file) TEST_MSON_(file, true)


TEST_MSON("primitives");
TEST_MSON("inheritance");
TEST_MSON("typed-array");
TEST_MSON("typed-object");
TEST_MSON("nontyped-object");
TEST_MSON("enum");
TEST_MSON("oneof");
TEST_MSON("mixin");
TEST_MSON_("mixin-nonexistent", false); // can fail, nonvalid blueprint
TEST_MSON("string-sample"); 
TEST_MSON("typed-array-sample"); 
TEST_MSON("group"); 
TEST_MSON("empty-sample"); 
TEST_MSON("inner-inheritance"); 
TEST_MSON("oneof-sample"); 
TEST_MSON("multiline-description"); 
TEST_MSON("primitive-variables"); 
TEST_MSON("named-with-types"); 
TEST_MSON("nontyped-array"); 
TEST_MSON("number-wrong-value"); 
TEST_MSON("enum-sample"); 
TEST_MSON_("primitive-with-members", false); // can fail, nonvalid blueprint
TEST_MSON("nontyped-array-sample");
TEST_MSON("resource-anonymous");
TEST_MSON("resource-nested-inheritance");
TEST_MSON("resource-nested-mixin");
TEST_MSON("resource-unresolved-reference");
TEST_MSON("resource-resolve-basetype");
TEST_MSON("resource-primitive-mixin");
TEST_MSON("array-typed-content");
TEST_MSON("resource-nested-member");
TEST_MSON("array-sample");
TEST_MSON("object-sample");
TEST_MSON("enum-members-description");
TEST_MSON("array-reference");
TEST_MSON("reference-override");
TEST_MSON("enum-variants");

#undef TEST_MSON
