![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

# Snowcrash

## C++ library

```c++
#include "snowcrash.h"

mdp::ByteBuffer blueprint = R"(
# My API
## GET /message
+ Response 200 (text/plain)

        Hello World!
)";

snowcrash::ParseResult<snowcrash::Blueprint> ast;
snowcrash::parse(blueprint, 0, ast);

std::cout << "API Name: " << ast.node.name << std::endl;
```

Refer to [`Blueprint.h`](src/Blueprint.h) for the details about the Snow Crash AST and [`BlueprintSourcemap.h`](src/BlueprintSourcemap.h) for details about Source Maps tree.
