![logo](https://raw.github.com/apiaryio/api-blueprint/master/assets/logo_apiblueprint.png)

# Drafter [![Circle CI](https://circleci.com/gh/apiaryio/drafter/tree/master.svg?style=shield)](https://circleci.com/gh/apiaryio/drafter/tree/master) [![Build status](https://ci.appveyor.com/api/projects/status/7t9qvldmui401dwe/branch/master?svg=true)](https://ci.appveyor.com/project/Apiary/drafter/branch/master)

Snowcrash parser harness

### API Blueprint Parser
Drafter is complex builder of [API Blueprint](http://apiblueprint.org). Internally it uses [Snowcrash library](https://github.com/apiaryio/snowcrash), reference [API Blueprint](http://apiblueprint.org) parser.

API Blueprint is Web API documentation language. You can find API Blueprint documentation on the [API Blueprint site](http://apiblueprint.org).

Drafter also provides the user ability to select the type of the output. There are two possible values:

* **API Elements Parse Result:** Parse Result is defined in API Elements according to [Parse Result Namespace](http://api-elements.readthedocs.io/en/latest/element-definitions/#parse-result-elements).
* **Normal AST Parse Result:** Parse Result defined by the [API Blueprint AST](https://github.com/apiaryio/api-blueprint-ast) Parse Result. The AST is deprecated and only available in the Drafter command line tool.

By default, Drafter assumes the Refract Parse Result.

Both the types of Parse Results are available in two different serialization formats, YAML and JSON. YAML is the default for the CLI.

## Status
- [Format 1A9](https://github.com/apiaryio/api-blueprint/releases/tag/format-1A9) fully implemented

## Install
OS X using Homebrew:

```sh
$ brew install drafter
```

[AUR package](https://aur.archlinux.org/packages/drafter/) for Arch Linux.

Other systems refer to [installation notes](#installation).

## Usage

Drafter is both a library and a command line tool.

### Command line tool

The command line tool allows you to parse a blueprint and/or check the validity
of a blueprint.

```shell
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

### C/C++ API

```c
#include <drafter/drafter.h>
```

The [header](https://github.com/apiaryio/drafter/blob/master/src/drafter.h) itself is annotated with comments. [C API unit tests](https://github.com/apiaryio/drafter/blob/master/test/test-CAPI.c) provide more examples.

#### Parse API Blueprint into API Elements

The `drafter_parse_blueprint_to` function translates a buffered API blueprint into [API
Elements](https://apielements.org/), serialized into one of its supported serialization formats.

```c
drafter_error drafter_parse_blueprint_to(
    const char* source,
    char** out,
    const drafter_parse_options* parse_opts,
    const drafter_serialize_options* serialize_opts);
);
```

Given a pointer to a UTF-8 encoded c-string,

```c
const char* blueprint =
  "# My API\n"
  "## GET /message\n"
  "+ Response 200 (text/plain)\n"
  "\n"
  "      Hello World!\n";
```

##### Serialized as YAML

Without options, the resulting API Elements is serialized as YAML.

```c
char* yamlApie = NULL;
if (DRAFTER_OK == drafter_parse_blueprint_to(blueprint, &yamlApie, NULL, NULL)) {
    printf("%s\n", yamlApie);
}

free(yamlApie);
```

##### Serialized as JSON

Tweaking `drafter_serialize_options` allows serialization into JSON.

```c
drafter_serialize_options* options = drafter_init_serialize_options();
drafter_set_format(options, DRAFTER_SERIALIZE_JSON);

char* jsonApie = NULL;
if (DRAFTER_OK == drafter_parse_blueprint_to(blueprint, &jsonApie, NULL, NULL)) {
    printf("%s\n", jsonApie);
}

free(jsonApie);
drafter_free_options(options);
```

#### Validate API Blueprint

API Blueprint can be validated via `drafter_check_blueprint`. 

```c
drafter_error drafter_check_blueprint(
    const char* source,
    drafter_result** res,
    const drafter_parse_options* parse_opts);
```

##### Simple validation

The return value of `drafter_check_blueprint` indicates validation success.

```c
drafter_result* result = NULL;
if (DRAFTER_OK == drafter_check_blueprint(blueprint, result)) {
    printf("Understood.\n");
}

drafter_free_result(result);
```

##### Access warnings and errors

After running `drafter_check_blueprint`, the `result` parameter is set to
reference an API Element containing validation warnings or errors.

Because the result is an API Element - `drafter_result` - it can be serialized
as such.

```c
drafter_result* result = NULL;
drafter_check_blueprint(blueprint, result);

if(result) {
    char* yamlApie = drafter_serialize(result, NULL);
    printf("%s\n", yamlApie);
    free(yamlApie);
}

drafter_free_result(result);
```

Serialization of API Elements as JSON is achieved by tweaking
`drafter_serialize_options` as discussed [here](#Serialized-as-JSON).

## Installation

Building Drafter will require a modern C++ compiler and
[CMake](https://cmake.org/install/). The following compilers are tested and
known to work:

| Compiler | Minimum Version |
|----------|-----------------|
| Clang    | 4.0             |
| GCC      | 5.3             |
| MSVC++   | 2015            |

The following steps can be used to build and install Drafter:

1. Download a stable release of Drafter (release tarballs can be found
   in [GitHub Releases](https://github.com/apiaryio/drafter/releases)):

    ```sh
    $ curl -OL <url to drafter release from GitHub releases>
    $ tar xvf drafter.tar.gz
    $ cd drafter
    ```

    Alternatively, you can clone the source repository, for example:

    ```sh
    $ git clone --recursive https://github.com/apiaryio/drafter.git
    $ cd drafter
    ```

2. Build & Install Drafter:

    POSIX (macOS/Linux):

    ```sh
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ [sudo] make install
    ```

    NOTE: You can use `cmake -DCMAKE_INSTALL_PREFIX="$HOME/.local ..` if you
    don't want a system wide install.

    Windows:

    ```sh
    > mkdir build
    > cd build
    > cmake ..
    > cmake --build . --target drafter --config Release
    ```

    On Windows, `drafter.exe` can be found inside `src\Release`

3. You can now use Drafter CLI and library:

    ```sh
    $ drafter --help
    ```

## Bindings

Drafter bindings in other languages:

- [drafter-npm](https://github.com/apiaryio/drafter-npm) (Node.js)
- [drafter.js](https://github.com/apiaryio/drafter.js) (Pure JavaScript)
- [RedSnow](https://github.com/apiaryio/redsnow) (Ruby)
- [DrafterPy](https://github.com/menecio/drafterpy) (Python)

### CLI Wrapper

- [fury-cli](https://github.com/apiaryio/fury-cli) (Node.js)
- [Drafter-php](https://github.com/hendrikmaus/drafter-php) (PHP)

## Contribute

Fork & Pull Request

If you want to create a binding for Drafter please refer to the [Writing a Binding](https://github.com/apiaryio/drafter/wiki/Writing-a-binding) article.

## License

MIT License. See the [LICENSE](https://github.com/apiaryio/drafter/blob/master/LICENSE) file.
