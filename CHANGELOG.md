# Drafter Changelog

## 5.0.0 (2020-04-20)

### Enhancements

* Drafter contains two new options for disabling messageBody and
  messageBodySchema generation from MSON. See the APIs `skip_gen_bodies` and
  `skip_gen_body_schemas` respectively.

## 5.0.0-rc.1 (2020-03-16)

### Breaking

* Parsing and serialisation options in the C API of Drafter contain breaking
  changes. Direct access to the option structures are no longer possible, a new
  API layer has been added for the options. See `drafter_init_parse_options`
  and `drafter_serialize_options` respectively.

### Enhancements

* Improved support for URI Template. Drafter supports up to, and including
  [level 4 of URI Template](https://tools.ietf.org/html/rfc6570#section-1.2).
  This includes support for the URI Template explode (`*`) modifier, and
  reserved value operator (`+`), and others.

  [#553](https://github.com/apiaryio/drafter/issues/553)
  [#456](https://github.com/apiaryio/drafter/issues/456)
  [#630](https://github.com/apiaryio/drafter/issues/630)
  [#666](https://github.com/apiaryio/drafter/issues/666)

* Added support for JSON Schema Draft 7 in schema generation from MSON.
  JSON Schemas are now generated using Draft 7 instead of Draft 4.

## 4.1.0 (2019-12-28)

### Enhancements

- Added support for dynamically linking against Drafter. The installation of
  Drafter will now install a dynamic library (dll/so/dylib) on the system.

## 4.0.2 (2019-10-29)

### Bug Fixes

* Avoid crashes on unresolved references, especially when MSON mixins reference
  primitive types (e.g. `Include (object)`).
  [#752](https://github.com/apiaryio/drafter/issues/752)

* Prevent crashes under some circumstances while handling incorrectly indented
  blocks.
  [#755](https://github.com/apiaryio/drafter/issues/755)

* Fixed cases where an enum with a matching sample, default or enumeration
  value would issue a duplication warning and the incorrect description may be
  selected.
  [#749](https://github.com/apiaryio/drafter/issues/749)

## 4.0.1 (2019-09-17)

### Bug Fixes

* Resolves an incompatibility with Microsoft Visual Studio 2019 which
  preventing Drafter from compiling. Resolves incompatibilities with older
  versions of Microsoft Visual Studio which caused run-time crashes.

* Numerous improvements to the correctness of the content type parsing which is
  used to detect JSON and JSON Schema content types. This includes fixing the
  detection of media types as case-insensitive, previously `application/JSON`
  would not have been identified as a JSON type unlike the [naming requirements
  for comparison in RFC 6838](https://tools.ietf.org/html/rfc6838#section-4.2).

* Certain warnings and errors now contain more compact source maps in cases
  where the annotation spanned multiple indented lines. This can improve the
  parse result size and amount of memory used by the parser and surrounding
  tooling when encountering warnings or errors which contain incorrectly
  indented blocks.

## 4.0.0 (2019-07-30)

### Bug Fixes

* Avoid duplication in `enumerations` for same elements with different description
  [#718](https://github.com/apiaryio/drafter/issues/719)

## 4.0.0-pre.8 (2019-07-01)

### Enhancements

* Adds de-duplication of objects in `anyOf` in produced JSON Schemas under
  cases where the properties have been declared in different orders in MSON.
  [#664](https://github.com/apiaryio/drafter/issues/664)

### Bug Fixes

* Fix escaping keys in generated JSON/JSON-Scheme in generated assets
  [#719](https://github.com/apiaryio/drafter/issues/719)

* When using a type hint with an array, for example `array[Unknown]`. We will now
  emit a warning when the referenced type does not exist. The invalid type
  reference will not be present in the parse result.

* Prevents Drafter from aborting while parsing some API Blueprints due to
  invalid internal state.

## 4.0.0-pre.7 (2019-05-31)

### Enhancements

* The default build type is now release when using cmake.

### Bug Fixes

* JSON value & JSON schema backends now support top-level Reference Elements.
  This prevents crashes on `Include` statements in `enum` contexts.
  [#709](https://github.com/apiaryio/drafter/issues/709)

## 4.0.0-pre.6 (2019-05-20)

### Bug Fixes

* The `fixed` type attribute now inherits to empty objects. This regression was
  introduced in 4.0.0-pre.1.
  [#686](https://github.com/apiaryio/drafter/issues/686)

* Prevents a segmentation fault while parsing a document which contains an
  "empty" `One Of` MSON entry.
  [#699](https://github.com/apiaryio/drafter/issues/699)

* Prevent an infinite loop while parsing a document with a circular MSON
  reference.
  [#702](https://github.com/apiaryio/drafter/issues/702)

## 4.0.0-pre.5 (2019-05-07)

### Bug Fixes

* Drafter now correctly expands types nested in EnumElement
  [#689](https://github.com/apiaryio/drafter/issues/689)

## 4.0.0-pre.4 (2019-04-25)

### Bug Fixes

* Drafter no longer attempts to interpret non-primitive MSON type hints as fallbacks for nested values.
  [#682](https://github.com/apiaryio/drafter/issues/682)

  ```apib
  + (array[object])
    + MSON string now interpreted as String Element instead of Object Element
  ```

* Release builds of Drafter will no longer include assertions, this prevents
  the potential for development intended assertions to cause Drafter to abort.

## 4.0.0-pre.3 (2019-04-08)

### Enhancements

* generated JSON Schema makes use of the `enum` validation instead of the more syntax heavy `anyOf` for each enumerations EnumElement entry typing a single value

* transitioned from C++14 back to C++11 to support older compilers

## 4.0.0-pre.2

### Bug Fixes

* Parameters which are of `enum` type will now have their `default` values
  correctly serialized in the output using `enum` type

### Enhancements

* Add column/line info to anotations source maps 

## 4.0.0-pre.1

### Breaking

* This release contains numerous changes to enumerations which affect how
  enumerations are serialised in API Elements.
    * See [API Elements 1.0](http://apielements.org/) for further specification
      on the format of the enumerations.
    * Samples are now collected as collection of individual elements.
    * The parser will now emit a warning when you define multiple default
      values for an enumeration.

* Descriptions for MSON structures are no longer found in generated JSON Schemas.

### Enhancements

* Significant performance improvements have been made to this release and
  parse times for API Blueprints are halved.

### Bug Fixes

* Large numbers will no longer be clipped and numbers will be rendered how
  they was described. For example `10.00` will be rendered as `10.00` and not `10`.
  [#443](https://github.com/apiaryio/drafter/issues/443)

* JSON Schema will no longer contain redundant anyOf entries when using
  fixed-type arrays.
  [#566](https://github.com/apiaryio/drafter/issues/566)

* Restored handling API Blueprints where referencing an unknown type inside an
  `array[]` would cause an assertion failure instead of a parsing error.
  [#556](https://github.com/apiaryio/drafter/issues/556)

* Fix parsing MSON object structures where a property is declared without a
  key. This was causing an assertion failure instead of a parse error.
  [#547](https://github.com/apiaryio/drafter/issues/547)

* JSON Schema generation will no longer include duplicate required properties
  when a property in MSON structure is duplicated.
  [#493](https://github.com/apiaryio/drafter/issues/493)

## 4.0.0-pre.0

### Breaking

* The following compiler versions are supported:

  * Microsoft Visual C++ 2015 or higher
  * GCC 5.3 or higher
  * Clang 4.0 or higher

* Deprecated API Blueprint AST support has been removed.

* Drafter will now produce API Elements 1.0 using the Refract 1.0 JSON Serialisation format.
  [Refract JSON Serialisation](https://github.com/refractproject/refract-spec/blob/master/formats/json-refract.md)
  [API Elements 1.0](http://apielements.org/)

### Enhancements

* Instead of returning int, functions that may error return `drafter_error`
  type. This adds additional type-safety when handling errors.

### Bug Fixes

* Fix JSON Schema "required" for multiple defined members
  [#493](https://github.com/apiaryio/drafter/issues/493)

## 3.2.7

### Bug Fixes

* Fix JSON Schema for OneOf when required is present.
  [#453](https://github.com/apiaryio/drafter/issues/453)

* Allow fixed-type when the nested type in array is six characters.
  [#463](https://github.com/apiaryio/drafter/issues/463)

* Fix a few issues with codeblocks format.

## 3.2.6

### Bug Fixes

* Fix JSON Schema support for nullable with enums.
  [#455](https://github.com/apiaryio/drafter/issues/455)

## 3.2.5

### Bug Fixes

* Action identifiers may now contain `(` and `)` characters.

  ```apib
  ## My Action (Deprecated) [GET]
  ```

## 3.2.4

### Bug Fixes

* Parameters values in API Elements are always serialized as API Element
  strings. This ensure consistency with API Blueprint AST and to prevent
  values being serialized differently. For example boolean of value `1` in API
  Blueprint AST would be treated as a string and therefore be serialized as
  `1`. However in API Elements it wouldn't be treated as truthy since `1` is
  not equal to `true` and would be serialized as `false`.

## 3.2.3

### Bug Fixes

* Prevent exposing duplicate parameters in API Elements output when a
  parameter is overshadowed.

* Fix support for escaped MSON-style parameters.
  [#445](https://github.com/apiaryio/drafter/issues/445)

## 3.2.2

### Bug Fixes

* Ensure that escaped property names in MSON are not treated as markdown.
  [snowcrash#422](https://github.com/apiaryio/snowcrash/pull/422)

## 3.2.1

### Bug Fixes

* Properly handle MD "code fences" while parsing headers (related to
  changes in sundown paser)

* fix crash - changes in sundown leads to crash for undisclosed item list

## 3.2.0

### Enhancements

* Improved sourcemaps of warnings when encountering malformed headers

* Added warning when default and sample is specified without value

* Improved JSON Schema regarding defaults
  [#424](https://github.com/apiaryio/drafter/issues/424)

## 3.1.3

### Bug Fixes

* Handle `fixed-type` with variable property names when generating JSON
  Schemas.
  [#390](https://github.com/apiaryio/drafter/issues/390)

* Drafter will now check the validity of header names and provide a warning
  for invalid characters found within a header name. Invalid headers will be
  skipped in the parse outputs.
  [#382](https://github.com/apiaryio/drafter/issues/382)

* Fix handling of escaped parameter identifiers with the older non-MSON
  parameter syntax.
  [#391](https://github.com/apiaryio/drafter/issues/391)

* Rectify JSON Schema generation for variable property names
  [#367](https://github.com/apiaryio/drafter/issues/367)

## 3.1.2

### Enhancements

* Add missing sourcemaps for named type references
  [#386](https://github.com/apiaryio/drafter/issues/386)

* Warn user on "action" with non-absolute path
  [#350](https://github.com/apiaryio/drafter/issues/350)

### Bug Fixes

* Prevent crashing and improve structure of result from parsing regarding
  markdown lists.

* Correctly set the content type for custom JSON Schema.
  [#392](https://github.com/apiaryio/drafter/issues/392)

* Fix the JSON Schema generated when using `fixed-type` attribute
  [#389](https://github.com/apiaryio/drafter/issues/389)

* Fix and improve URI Template parameters validation
  [#402](https://github.com/apiaryio/drafter/issues/402)

## 3.1.1

### Bug Fixes

* Prevent crashing when no default value is specified for enum members

## 3.1.0

### Enhancements

* Warn when primitive values in MSON doesn't follow the given type format
  [#334](https://github.com/apiaryio/drafter/issues/334)

### Bug Fixes

* While overriding a property on an inherited object, the description and type
  annotation was not being overridden.
  [#366](https://github.com/apiaryio/drafter/issues/366)

* Always give an error when unable to find named type.
  [snowcrash#354](https://github.com/apiaryio/snowcrash/issues/354)

* Ensure that escaped values in MSON are not treated as markdown
  [#365](https://github.com/apiaryio/drafter/issues/365)

* Fixes detection of parameters using the MSON syntax when the description of a
  parameter includes `(` or `)`.
  [snowcrash#345](https://github.com/apiaryio/snowcrash/issues/345)


## 3.1.0-pre.0

### Enhancements

* Warn about reserved characters in Named type declaration.
  [snowcrash#335](https://github.com/apiaryio/snowcrash/issues/335)

* Add `fixed-type` keyword support.
  [mson#66](https://github.com/apiaryio/mson/issues/66)

* Add warnings when the given MSON type attributes contradict each other.

### Bug Fixes

* Ensure that sample values are rendered in JSON Schema when using the `fixed`
  attribute.
  [#343](https://github.com/apiaryio/drafter/issues/343)

* Prevents an empty properties section being treated as an unrecognized block.
  [#127](https://github.com/apiaryio/drafter/issues/127)
  [snowcrash#200](https://github.com/apiaryio/snowcrash/issues/200)

* Fixes rendering when One Of contains member sections with named types.
  [#360](https://github.com/apiaryio/drafter/issues/360)

* Default and sample enum values will take precedence as the example value in
  the rendered JSON structure over the enum members.

* Add type attributes for array values.
  [#193](https://github.com/apiaryio/drafter/issues/193)

* Value members with required will not have items in JSON Schema anymore.
  [#371](https://github.com/apiaryio/drafter/issues/371)

* Ensures that MSON validation warnings are present when generating APIB AST.

* Improved validation of `Request` keyword matching.

## 3.0.0

### Breaking

* Drafter now uses C++11.

  The following compiler versions are supported:

  * Microsoft Visual C++ 2013 or higher
  * GCC 4.8 or higher
  * Clang 3.5 or higher

### Enhancements

* Drafter now provides a full C and C++ API interface, you may now validate,
  parse and serialize an API Blueprint. Please see `drafter.h` header for full
  API documentation.

* Improve error messages for a few blueprint errors

### Bug Fixes

* Correctly render named primitive types when used as a nested or member
  type such as `items (array[Other Type])` in JSON.
  [#312](https://github.com/apiaryio/drafter/issues/312)

* Fixes an issue where defining an implicit object data structure would raise
  an invalid syntax warning and not be parsed.
  [#318](https://github.com/apiaryio/drafter/issues/318)

* Prevent causing an infinite loop when parsing a resource which inherits from
  itself.
  [#328](https://github.com/apiaryio/drafter/issues/328)

* Correctly parse object samples within arrays.
  [#246](https://github.com/apiaryio/drafter/issues/246)

* Correctly render JSON Schema when `fixed` is used on attributes section or named type [mson#60](https://github.com/apiaryio/mson/issues/60)


## 2.3.1

### Bug Fixes

* Fixes a bug when a referenced mixin inherits another named type
  [#254](https://github.com/apiaryio/drafter/issues/254)

* Some of the errors have been changed to warnings

* Fixes "One Of" JSON rendering. This fix does not include JSON schema
  for multiple `One of` definition in MSON object.
  [#288](https://github.com/apiaryio/drafter/issues/288)


## 2.3.0

### Bug Fixes

* Parameters in Refract output will no longer contain a `content` value
  when there is no sample value.
  [#286](https://github.com/apiaryio/drafter/issues/286)

* Errors raised from Snowcrash, the underlying parser are now treated
  as errors. This resolves an issue where Drafter would get into an
  infinite loop when certain circular references are found in MSON
  Data Structures.
  [#255](https://github.com/apiaryio/drafter/issues/255)

* Errors raised from drafter are now correctly treated as errors,
  previously they were exposed as warnings.

* Improved error reporting with source maps when using cdrafter.

* Resolved an issue when using a mixins from implicit objects would raise a
  warning that the implicit object was not an object.
  [#138](https://github.com/apiaryio/drafter/issues/138)
  [#243](https://github.com/apiaryio/drafter/issues/243)


## 2.3.0-pre.2

This release restores an API that caused a breaking change in the
previous pre-release 2.3.0-pre.1.

## 2.3.0-pre.1

### Bug Fixes

* It was causing lot of different problems like segmentation faults, stack
  overflows or even infinite loops depending on what error was ignored.
  [#269](https://github.com/apiaryio/drafter/issues/269)

* Variable property name issues are fixed including correct generation
  of JSON Schema.

* Improved error reporting. Some of the semantic errors like circular
  references were issued without a sourcemap information, that is not
  the case anymore.

* `httpRequest` and `httpResponse` payloads now have source maps attached while
  using the Refract output.
  [#259](https://github.com/apiaryio/drafter/issues/259)


## 2.3.0-pre.0

### Enhancements

* Partial implementation of
  [RFC 4: Request Parameters](https://github.com/apiaryio/api-blueprint-rfcs/blob/master/rfcs/0004-request-parameters.md)
  has been added. Request parameters are now parsed, however validation
  of these parameters is not yet handled.

* Adds support for building Drafter with Microsoft Visual Studio 2015.

### Bug Fixes

* The content-type for JSON Schema messageBodySchema is now correctly
  `application/schema+json`.

* Fixes a problem while rendering JSON Schema may have resulted in an
  `ExtendElement` warning.
  [#217](https://github.com/apiaryio/drafter/issues/217)


## 2.2.0

### Enhancements

* The JavaScript API has been aligned with the Protagonist
  binding, allowing you to use Drafter JS and Protagonist
  with the same interface.

### Bug Fixes

* Fixes a crash when handling circular references in MSON Mixins.
