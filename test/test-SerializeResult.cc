#include "draftertest.h"
#include "SerializeResult.h"

using namespace draftertest;

TEST_AST_SOURCE_MAP("result", "mson");
TEST_AST_SOURCE_MAP("result", "blueprint");
TEST_REFRACT("result", "blueprint");
