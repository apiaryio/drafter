{
  "includes": [
    "ext/snowcrash/common.gypi"
  ],
  "targets" : [
    {
      'target_name': 'libsos',
      'type': 'static_library',
      'include_dirs': [
        'ext/sos/src'
      ],
      'sources': [
        'ext/sos/src/sos.cc',
        'ext/sos/src/sos.h',
        'ext/sos/src/sosJSON.h',
        'ext/sos/src/sosYAML.h'
      ]
    },
    {
      "target_name": "libdrafter",
      'type': '<(libdrafter_type)',
      "sources": [
        "src/drafter.h",
        "src/drafter.cc",
        "src/cdrafter.h",
        "src/cdrafter.cc",

        "src/stream.h",
        "src/Version.h",

        "src/NodeInfo.h",
        "src/Serialize.h",
        "src/Serialize.cc",
        "src/SerializeAST.h",
        "src/SerializeAST.cc",
        "src/SerializeSourcemap.h",
        "src/SerializeSourcemap.cc",
        "src/SerializeResult.h",
        "src/SerializeResult.cc",
        "src/RefractAPI.h",
        "src/RefractAPI.cc",
        "src/RefractDataStructure.h",
        "src/RefractDataStructure.cc",
        "src/RefractSourceMap.h",
        "src/RefractSourceMap.cc",
        "src/Render.h",
        "src/Render.cc",
        "src/NamedTypesRegistry.cc",
        "src/NamedTypesRegistry.h",

        # librefract parts - will be separated into other project

        "src/refract/Element.h",
        "src/refract/Element.cc",
        "src/refract/ElementFwd.h",
        "src/refract/Typelist.h",
        "src/refract/VisitableBy.h",

        "src/refract/Visitor.h",
        "src/refract/Visitors.h",
        "src/refract/VisitorUtils.h",
        "src/refract/VisitorUtils.cc",

        "src/refract/SerializeCompactVisitor.h",
        "src/refract/SerializeCompactVisitor.cc",
        "src/refract/SerializeVisitor.h",
        "src/refract/SerializeVisitor.cc",
        "src/refract/ComparableVisitor.h",
        "src/refract/ComparableVisitor.cc",
        "src/refract/TypeQueryVisitor.h",
        "src/refract/TypeQueryVisitor.cc",
        "src/refract/IsExpandableVisitor.h",
        "src/refract/IsExpandableVisitor.cc",
        "src/refract/ExpandVisitor.h",
        "src/refract/ExpandVisitor.cc",
        "src/refract/RenderJSONVisitor.h",
        "src/refract/RenderJSONVisitor.cc",
        "src/refract/PrintVisitor.h",
        "src/refract/PrintVisitor.cc",
        "src/refract/JSONSchemaVisitor.h",
        "src/refract/JSONSchemaVisitor.cc",
        "src/refract/ApplyVisitor.h",
        "src/refract/ApplyVisitor.cc",

        "src/refract/Registry.h",
        "src/refract/Registry.cc",

        "src/refract/Build.h",

        "src/refract/AppendDecorator.h",
        "src/refract/ElementInserter.h",
        "src/refract/Query.h",
        "src/refract/Iterate.h",
      ],
      # FIXME: replace by direct dependecies
      "include_dirs": [
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/sos/src",
        "ext/snowcrash/src",
      ],
      "dependencies": [
        "libsos",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
      ],
    },
    {
      'target_name': 'test-libdrafter',
      'type': 'executable',
      'include_dirs': [
        'src',
        'test',
        'test/vendor/Catch/include',
        'test/vendor/dtl/dtl',
        "ext/snowcrash/src",
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/sos/src",
      ],
      'sources': [
        "test/test-drafter.cc",
        "test/test-cdrafter.cc",
        "test/test-SerializeResult.cc",
        "test/test-RefractDataStructureTest.cc",
        "test/test-RefractAPITest.cc",
        "test/test-RefractParseResultTest.cc",
        "test/test-RenderTest.cc",
        "test/test-RefractSourceMapTest.cc",
        "test/test-SchemaTest.cc",
        "test/test-CircularReferenceTest.cc",
        "test/test-ApplyVisitorTest.cc",
      ],
      'dependencies': [
        "libdrafter",
      ],
      'conditions': [
         [ 'OS=="win"', { 'defines' : [ 'WIN' ] } ]
      ],
    },
    {
      "target_name": "drafter",
      "type": "executable",
      "sources": [
        "src/main.cc",
        "src/config.cc",
        "src/config.h",
        "src/reporting.cc",
        "src/reporting.h",
      ],
      # FIXME: replace by direct dependecies
      "include_dirs": [
        "ext/cmdline",
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/sos/src",
        "ext/snowcrash/src",
      ],
      "dependencies": [
        "libdrafter",
      ],
    },
  ],
}
