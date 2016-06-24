# Drafter Changelog

## Master

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
