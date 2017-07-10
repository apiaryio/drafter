This document describes the architecture of the API Blueprint parser (Drafter) and it's subcomponents.

### Markdown

Since API Blueprint is designed on top of Markdown, the smallest base of the parser contains a Markdown parser called [Sundown](https://github.com/apiaryio/sundown) which we use to create an AST of the markdown.

A node in the AST is made up of `mdp::MarkdownNode` from [markdown-parser](https://github.com/apiaryio/markdown-parser) and depending on the the type of node, it can have children. This node will contain all the original data from the source document (including things like sourcemap).

### Sos

[Sos](http://github.com/apiaryio/sos) is a serialization helper library which serializes the given object into multiple formats like JSON and YAML.

### Snowcrash

[Snowcrash](https://github.com/apiaryio/snowcrash) takes the markdown AST and parses it into an internal structure defined in [Blueprint.h](https://github.com/apiaryio/snowcrash/blob/master/src/Blueprint.h).

> `snowcrash::parse` takes the document directly and creates the markdown AST

The output of `snowcrash::parse` is `snowcrash::ParseResult` which is basically nothing but a combination of `snowcrash::Blueprint` as `node` and `snowcrash::BlueprintSourcemap` as `sourceMap` along with `report`.

The source map format used in Snow Crash output is a set of semicolon-separated number pairs. Every pair represent a continuous range in the `:<character index>:<character count>` format.

Note a warning can span across several non-continuous blocks.

*TODO*: MSON Inheritance

*TODO*: Section Processor format

### Drafter

[Drafter](https://github.com/apiaryio/drafter) contains a CLI interface which takes in the API Blueprint document along with options and uses snowcrash internally to get a `snowcrash::ParseResult`. This parse result is then converted into internal refract representation format by `drafter::WrapRefract`.

*TODO*: Refract library

All the MSON is expanded with `drafter::ExpandRefract` during the conversion which is used to generate the JSON Schema/Body for Payloads that contained Attributes.

The whole refract tree is then serialized into `sos::Object` from `drafter::SerializeRefract` which is then converted into appropriate format as requested.
