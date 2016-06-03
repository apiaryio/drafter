#include "draftertest.h"

using namespace draftertest;

TEST_AST("parse-result", "simple");
TEST_AST("parse-result", "warning");
TEST_AST("parse-result", "warnings");
TEST_AST("parse-result", "error-warning");
TEST_AST("parse-result", "blueprint");
TEST_AST("parse-result", "mson");
