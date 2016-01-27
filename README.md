![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

# Drafter [![Circle CI](https://circleci.com/gh/apiaryio/drafter/tree/master.svg?style=shield)](https://circleci.com/gh/apiaryio/drafter/tree/master) [![Build status](https://ci.appveyor.com/api/projects/status/7t9qvldmui401dwe/branch/master?svg=true)](https://ci.appveyor.com/project/Apiary/drafter/branch/master)

Snowcrash parser harness

### API Blueprint Parser
Drafter is complex builder of [API Blueprint](http://apiblueprint.org). Internally it uses [Snowcrash library](https://github.com/apiaryio/snowcrash), reference [API Blueprint](http://apiblueprint.org) parser.

API Blueprint is Web API documentation language. You can find API Blueprint documentation on the [API Blueprint site](http://apiblueprint.org).

Additionally Drafter provide set of Wrappers for serialization, of parsing result, via  [SOS library](https://github.com/apiaryio/sos) into JSON and YAML format.

Drafter also provides the user ability to select the type of the output. There are two possible values:

* **Refract Parse Result:** Parse Result is defined in Refract elements according to [Parse Result Namespace](https://github.com/refractproject/refract-spec/blob/master/namespaces/parse-result-namespace.md)
* **Normal AST Parse Result:** Parse Result defined by the [API Blueprint AST](https://github.com/apiaryio/api-blueprint-ast) Parse Result.

By default, Drafter assumes the Refract Parse Result.

Both the types of Parse Results are available in two different serialization formats, YAML and JSON. YAML is the default for the CLI.

## Status
- [Format 1A9](https://github.com/apiaryio/api-blueprint/releases/tag/format-1A9) fully implemented

## Install
OS X using Homebrew:

```sh
$ brew install --HEAD \
  https://raw.github.com/apiaryio/drafter/master/tools/homebrew/drafter.rb
```

[AUR package](https://aur.archlinux.org/packages/drafter-git/) for Arch Linux.

Other systems refer to [build notes](#build).

## Use

### C++ library
```c++
#include "drafter.h"         // Blueprint Parser
#include "SerializeResult.h" // Result Wrapper for serialization
#include "sosJSON.h"         // Serializer

mdp::ByteBuffer blueprint = R"(
# My API
## GET /message
+ Response 200 (text/plain)

        Hello World!
)";

// Blueprint parsing
snowcrash::ParseResult<snowcrash::Blueprint> ast;
drafter::ParseBlueprint(blueprint, 0, ast);

std::cout << "API Name: " << ast.node.name << std::endl;

// Serialization to JSON format
sos::SerializeJSON serializer;
serializer.process(drafter::WrapResult(ast.node, drafter::WrapperOptions(drafter::RefractASTType)), std::cout);

```

### C-interface

For purpose of [bindings](#bindings) to other languages Drafter provides very simple C-interface.
```c
#include "cdrafter.h"

const char* source = "# My API\n## GET /message\n + Response 200 (text/plain)\n\n        Hello World\n";
char *result = NULL;
int ret = drafter_c_parse(source, 0, DRAFTER_REFRACT_AST_TYPE, &result);

printf("Result: %s\n", ret == 0 ? "OK" : "ERROR");
printf("Serialized JSON result:\n%s\n", result);

free(result); /* we MUST release allocted memory for result */
```

Refer to [`Blueprint.h`](https://github.com/apiaryio/snowcrash/blob/master/src/Blueprint.h) for the details about the Snow Crash AST and [`BlueprintSourcemap.h`](https://github.com/apiaryio/snowcrash/blob/master/src/BlueprintSourcemap.h) for details about Source Maps tree. See [Drafter bindings](#bindings) for using the library in **other languages**.


### Command line tool
```bash
$ cat << 'EOF' > blueprint.apib
# My API
## GET /message
+ Response 200 (text/plain)

        Hello World!
EOF

$ drafter blueprint.apib
element: "parseResult"
content:
  -
    element: "category"
    meta:
      classes:
        - "api"
      title: "My API"
...
```

See [parse feature](features/parse.feature) for the details on using the `drafter` command line tool.

## Build
1. Clone the repo + fetch the submodules:

    ```sh
    $ git clone --recursive git://github.com/apiaryio/drafter.git
    $ cd drafter
    ```

2. Build & test Drafter:

    ```sh
    $ ./configure
    $ make test
    ```

    To include integration tests (using Cucumber) use the `--include-integration-tests` flag:

    ```sh
    $ ./configure --include-integration-tests
    $ make test
    ```

We love **Windows** too! Please refer to [Building on Windows](https://github.com/apiaryio/drafter/wiki/Building-on-Windows).

### Drafter command line tool
1. Build `drafter`:

    ```sh
    $ make drafter
    ```

2. Install & use `drafter`:

    ```sh
    $ sudo make install
    $ drafter --help
    ```

## Bindings
Drafter bindings in other languages:

- [Protagonist](https://github.com/apiaryio/protagonist) (Node.js)
- [RedSnow](https://github.com/apiaryio/redsnow) (Ruby)

### CLI Wrapper
- [Drafter-php](https://github.com/hendrikmaus/drafter-php) (PHP)

## drafter.js

*Note: drafter.js is work in progress, use only for experimental purposes*

`drafter.js` is a pure JavaScript version of the `drafter` library. It
exposes a single `parse` function which takes an API Blueprint string and options as input and returns the parse result.
It is built from the C++ sources using
[emscripten](http://kripken.github.io/emscripten-site/).

It can be downloaded from the [releases page](https://github.com/apiaryio/drafter/releases) or installed via `npm install drafter.js`.

```js
// Web browser
<script src="./drafter.js"></script>
<script src="./drafter.js.mem"></script>
// Node.js
var drafter = require('drafter.js');

// Example usage once loaded via one of the methods above:
try {
  var res = drafter.parse('# API Blueprint...', {exportSourcemap: true});
  console.log(res);
} catch (err) {
  console.log(err);
}
```

Supported options:

- `exportSourcemap`: Set to export sourcemap information.
- `json`: Set to `false` to disable parsing of the JSON data. You will
  instead get a JSON string as the result.
- `requireBlueprintName`: Set to generate an error if the blueprint is
  missing a title.
- `type`: Either `refract` (default) or `ast`.

`drafter.js` can serve as drop in replacement for
[protagonist](https://github.com/apiaryio/protagonist), it supports
the same API. So stuff like
```js
try {
  var protagonist = require('protagonist');
}
catch (e) {
  console.log("protagonist not found, using drafter.js.");
  var protagonist = require('drafter.js');
}

protagonist.parse(data, options, function(err, result) {
  if (err) {
    console.log(JOSN.stringify(err));
  }
  console.log(JSON.stringify(data));
});
```
is possible. Protagonist `parseSync` function is available too.

### Build drafter.js

*Unfortunately building drafter.js works only on a *nix environment at the moment.*

1. Building is easy using [Docker](https://www.docker.com/).

2. Build
    ```sh
    $ docker pull "apiaryio/base-emscripten-dev"
    $ docker run -v $(pwd):/src -t apiaryio/base-emscripten-dev emcc/emcbuild.sh
    ```

3. Check out the `./emcc/test-drafter.js` and `./emcc/test.html` files for example usage. You can also use `npm install` and then `npm test` to run the tests.

The resulting stand-alone library `drafter.js` is in the `./emcc` directory. Don't forget to serve the `drafter.js.mem` file as it is required by `drafter.js`. There is also a single-file version in `drafter.nomem.js` that can be used, but it may take longer to load in a web browser environment.

To get a debug version or version enabled to be used with `emrun` run
the `emcbuild.sh` script it with `-d` or `-e` respectively.

#### Squeeze the size

If you want to squeeze the size to a minimum install
[uglify-js](https://github.com/mishoo/UglifyJS2) and try running
`emcbuild.sh -u`, this will use `uglify-js` with compression, beware
that this might cause some errors, if you encounter them try
`drafter.js` without it to verify that it is caused by `uglify-js`
and report it please.

## Contribute
Fork & Pull Request

If you want to create a binding for Drafter please refer to the [Writing a Binding](https://github.com/apiaryio/drafter/wiki/Writing-a-binding) article.

## License
MIT License. See the [LICENSE](https://github.com/apiaryio/drafter/blob/master/LICENSE) file.
