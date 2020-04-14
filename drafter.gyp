{
  "includes": [
    "common.gypi"
  ],

  "targets" : [

# LIBSUNDOWN
    {
      'target_name': 'libsundown',
      'type': 'static_library',
      'include_dirs': [
        'packages/sundown/src',
        'packages/sundown/html'
      ],
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/sundown/src',
        ],
      },
      'sources': [
        'packages/sundown/src/autolink.c',
        'packages/sundown/src/autolink.h',
        'packages/sundown/src/buffer.c',
        'packages/sundown/src/buffer.h',
        'packages/sundown/src/html_blocks.h',
        'packages/sundown/src/markdown.c',
        'packages/sundown/src/markdown.h',
        'packages/sundown/src/src_map.c',
        'packages/sundown/src/src_map.h',
        'packages/sundown/src/stack.c',
        'packages/sundown/src/stack.h',
        'packages/sundown/html/houdini.h',
        'packages/sundown/html/houdini_href_e.c',
        'packages/sundown/html/houdini_html_e.c',
        'packages/sundown/html/html.c',
        'packages/sundown/html/html.h',
        'packages/sundown/html/html_smartypants.c'
      ]
    },

# LIBAPIB
    {
      'target_name': 'libapib',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/apib/include',
          'packages/variant/include',
          'packages/boost',
        ],
      },
      'include_dirs' : [
        'packages/apib/include',
        'packages/apib/src',
        'packages/variant/include',
        'packages/boost',
      ],
      'sources': [
        'packages/apib/src/MediaType.cc',
      ],
    },

# TEST-LIBAPIB
    {
      'target_name': 'test-libapib',
      'type': 'executable',
      'include_dirs': [
        'packages/Catch/single_include',
      ],
      'sources': [
        'packages/apib/test/test-MediaType.cc',
        'packages/apib/test/test-apib.cc',
      ],
      'dependencies': [
        'libapib',
      ]
    },

# LIBAPIB-PARSER
    {
      'target_name': 'libapib-parser',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/apib-parser/include',
          'packages/apib-parser/src/snowcrash',
          'packages/apib-parser/src/markdown-parser',
          'packages/apib-parser/src',
          'packages/variant/include',
          'packages/boost',
        ],
      },
      'include_dirs' : [
        'packages/boost',
        'packages/variant/include',
        'packages/PEGTL/include',
        'packages/apib-parser/include',
        'packages/apib-parser/src/snowcrash',
        'packages/apib-parser/src/markdown-parser',
        'packages/apib-parser/src',
      ],
      'export_dependent_settings': [
        'libsundown',
        'libapib'
      ],
      'sources': [
        'packages/apib-parser/src/MediaTypeParser.cc',
        'packages/apib-parser/src/grammar/Mediatype.h',
        'packages/apib-parser/src/grammar/recover.h',
        'packages/apib-parser/src/grammar/UriTemplateParser.h',
        'packages/apib-parser/src/markdown-parser/ByteBuffer.cc',
        'packages/apib-parser/src/markdown-parser/ByteBuffer.h',
        'packages/apib-parser/src/markdown-parser/MarkdownNode.cc',
        'packages/apib-parser/src/markdown-parser/MarkdownNode.h',
        'packages/apib-parser/src/markdown-parser/MarkdownParser.cc',
        'packages/apib-parser/src/markdown-parser/MarkdownParser.h',
        'packages/apib-parser/src/snowcrash/HTTP.cc',
        'packages/apib-parser/src/snowcrash/HTTP.h',
        'packages/apib-parser/src/snowcrash/MSON.cc',
        'packages/apib-parser/src/snowcrash/MSONOneOfParser.cc',
        'packages/apib-parser/src/snowcrash/MSONSourcemap.cc',
        'packages/apib-parser/src/snowcrash/MSONTypeSectionParser.cc',
        'packages/apib-parser/src/snowcrash/MSONValueMemberParser.cc',
        'packages/apib-parser/src/snowcrash/Blueprint.cc',
        'packages/apib-parser/src/snowcrash/BlueprintSourcemap.cc',
        'packages/apib-parser/src/snowcrash/Section.cc',
        'packages/apib-parser/src/snowcrash/Section.h',
        'packages/apib-parser/src/snowcrash/Signature.cc',
        'packages/apib-parser/src/snowcrash/Signature.h',
        'packages/apib-parser/src/snowcrash/snowcrash.cc',
        'packages/apib-parser/src/snowcrash/snowcrash.h',
        'packages/apib-parser/src/snowcrash/UriTemplateParser.cc',
        'packages/apib-parser/src/snowcrash/UriTemplateParser.h',
        'packages/apib-parser/src/snowcrash/PayloadParser.h',
        'packages/apib-parser/src/snowcrash/SectionParserData.h',
        'packages/apib-parser/src/snowcrash/ActionParser.h',
        'packages/apib-parser/src/snowcrash/AssetParser.h',
        'packages/apib-parser/src/snowcrash/AttributesParser.h',
        'packages/apib-parser/src/snowcrash/Blueprint.h',
        'packages/apib-parser/src/snowcrash/BlueprintParser.h',
        'packages/apib-parser/src/snowcrash/BlueprintSourcemap.h',
        'packages/apib-parser/src/snowcrash/BlueprintUtility.h',
        'packages/apib-parser/src/snowcrash/CodeBlockUtility.h',
        'packages/apib-parser/src/snowcrash/DataStructureGroupParser.h',
        'packages/apib-parser/src/snowcrash/HeadersParser.h',
        'packages/apib-parser/src/snowcrash/HeadersParser.cc',
        'packages/apib-parser/src/snowcrash/ModelTable.h',
        'packages/apib-parser/src/snowcrash/MSON.h',
        'packages/apib-parser/src/snowcrash/MSONSourcemap.h',
        'packages/apib-parser/src/snowcrash/MSONMixinParser.h',
        'packages/apib-parser/src/snowcrash/MSONNamedTypeParser.h',
        'packages/apib-parser/src/snowcrash/MSONOneOfParser.h',
        'packages/apib-parser/src/snowcrash/MSONParameterParser.h',
        'packages/apib-parser/src/snowcrash/MSONPropertyMemberParser.h',
        'packages/apib-parser/src/snowcrash/MSONTypeSectionParser.h',
        'packages/apib-parser/src/snowcrash/MSONUtility.h',
        'packages/apib-parser/src/snowcrash/MSONValueMemberParser.h',
        'packages/apib-parser/src/snowcrash/ParameterParser.h',
        'packages/apib-parser/src/snowcrash/ParametersParser.h',
        'packages/apib-parser/src/snowcrash/Platform.h',
        'packages/apib-parser/src/snowcrash/RegexMatch.h',
        'packages/apib-parser/src/snowcrash/RelationParser.h',
        'packages/apib-parser/src/snowcrash/ResourceGroupParser.h',
        'packages/apib-parser/src/snowcrash/ResourceParser.h',
        'packages/apib-parser/src/snowcrash/SectionParser.h',
        'packages/apib-parser/src/snowcrash/SectionProcessor.h',
        'packages/apib-parser/src/snowcrash/SignatureSectionProcessor.h',
        'packages/apib-parser/src/snowcrash/SourceAnnotation.h',
        'packages/apib-parser/src/snowcrash/StringUtility.h',
        'packages/apib-parser/src/snowcrash/ValuesParser.h',
      ],
      'conditions': [
        [ 'OS=="win"',
          { 'sources': [ 'packages/apib-parser/src/snowcrash/win/RegexMatch.cc' ] },
          { 'sources': [ 'packages/apib-parser/src/snowcrash/posix/RegexMatch.cc' ] } # OS != Windows
        ]
      ],
      'dependencies': [
        'libsundown',
        'libapib'
      ]
    },

# TEST-LIBAPIB-PARSER
    {
      'target_name': 'test-libapib-parser',
      'type': 'executable',
      'include_dirs': [
        'packages/Catch/single_include',
        'packages/PEGTL/include',
      ],
      'sources': [
        "packages/apib-parser/test/grammar/test-MediaType.cc",
        "packages/apib-parser/test/grammar/test-Recover.cc",
        "packages/apib-parser/test/grammar/test-UriTemplate.cc",
        'packages/apib-parser/test/markdown-parser/test-ByteBuffer.cc',
        'packages/apib-parser/test/markdown-parser/test-MarkdownParser.cc',
        'packages/apib-parser/test/snowcrash/test-ActionParser.cc',
        'packages/apib-parser/test/snowcrash/test-AssetParser.cc',
        'packages/apib-parser/test/snowcrash/test-AttributesParser.cc',
        'packages/apib-parser/test/snowcrash/test-Blueprint.cc',
        'packages/apib-parser/test/snowcrash/test-BlueprintParser.cc',
        'packages/apib-parser/test/snowcrash/test-BlueprintUtility.cc',
        'packages/apib-parser/test/snowcrash/test-DataStructureGroupParser.cc',
        'packages/apib-parser/test/snowcrash/test-HeadersParser.cc',
        'packages/apib-parser/test/snowcrash/test-Indentation.cc',
        'packages/apib-parser/test/snowcrash/test-ModelTable.cc',
        'packages/apib-parser/test/snowcrash/test-MSONMixinParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONNamedTypeParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONOneOfParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONParameterParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONPropertyMemberParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONTypeSectionParser.cc',
        'packages/apib-parser/test/snowcrash/test-MSONUtility.cc',
        'packages/apib-parser/test/snowcrash/test-MSONValueMemberParser.cc',
        'packages/apib-parser/test/snowcrash/test-ParameterParser.cc',
        'packages/apib-parser/test/snowcrash/test-ParametersParser.cc',
        'packages/apib-parser/test/snowcrash/test-PayloadParser.cc',
        'packages/apib-parser/test/snowcrash/test-RegexMatch.cc',
        'packages/apib-parser/test/snowcrash/test-RelationParser.cc',
        'packages/apib-parser/test/snowcrash/test-ResourceParser.cc',
        'packages/apib-parser/test/snowcrash/test-ResourceGroupParser.cc',
        'packages/apib-parser/test/snowcrash/test-SectionParser.cc',
        'packages/apib-parser/test/snowcrash/test-Signature.cc',
        'packages/apib-parser/test/snowcrash/test-StringUtility.cc',
        'packages/apib-parser/test/snowcrash/test-SymbolIdentifier.cc',
        'packages/apib-parser/test/snowcrash/test-UriTemplateParser.cc',
        'packages/apib-parser/test/snowcrash/test-ValuesParser.cc',
        'packages/apib-parser/test/snowcrash/test-Warnings.cc',
        'packages/apib-parser/test/snowcrash/test-snowcrash.cc',
        'packages/apib-parser/test/test-apib_parser.cc'
      ],
      'dependencies': [
        'libapib-parser',
      ]
    },

# TEST-LIBAPIB-PARSER-PERF
    {
      'target_name': 'test-libapib-parser-perf',
      'type': 'executable',
      'sources': [
        'packages/apib-parser/test/snowcrash/performance/perf-snowcrash.cc'
      ],
      'dependencies': [
        'libapib-parser',
      ]
    },

# LIBDRAFTER
    {
      "target_name": "libdrafter",
      'type': '<(libdrafter_type)',
      "conditions" : [
        [ 'libdrafter_type=="shared_library"', { 'defines' : [ 'DRAFTER_BUILD_SHARED' ] }, { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/drafter/src',
          'packages/boost',
          'packages/variant/include',
        ],
      },
      'include_dirs' : [
        'packages/boost',
        'packages/variant/include',
        'packages/drafter/src',
      ],
      'export_dependent_settings': [
        'libapib-parser',
        'libapib',
      ],
      "sources": [
        "packages/drafter/src/drafter.h",
        "packages/drafter/src/options.h",
        "packages/drafter/src/drafter.cc",
        "packages/drafter/src/options.cc",
        "packages/drafter/src/stream.h",
        "packages/drafter/src/Version.h",

        "packages/drafter/src/NodeInfo.h",
        "packages/drafter/src/Serialize.h",
        "packages/drafter/src/Serialize.cc",
        "packages/drafter/src/SerializeKey.h",
        "packages/drafter/src/SerializeKey.cc",
        "packages/drafter/src/SerializeResult.h",
        "packages/drafter/src/SerializeResult.cc",
        "packages/drafter/src/RefractAPI.h",
        "packages/drafter/src/RefractAPI.cc",
        "packages/drafter/src/MsonTypeSectionToApie.h",
        "packages/drafter/src/MsonTypeSectionToApie.cc",
        "packages/drafter/src/MsonMemberToApie.h",
        "packages/drafter/src/MsonMemberToApie.cc",
        "packages/drafter/src/MsonOneOfSectionToApie.h",
        "packages/drafter/src/MsonOneOfSectionToApie.cc",
        "packages/drafter/src/RefractDataStructure.h",
        "packages/drafter/src/RefractDataStructure.cc",
        "packages/drafter/src/RefractSourceMap.h",
        "packages/drafter/src/RefractSourceMap.cc",
        "packages/drafter/src/Render.h",
        "packages/drafter/src/Render.cc",
        "packages/drafter/src/NamedTypesRegistry.cc",
        "packages/drafter/src/NamedTypesRegistry.h",
        "packages/drafter/src/RefractElementFactory.h",
        "packages/drafter/src/RefractElementFactory.cc",
        "packages/drafter/src/ConversionContext.cc",
        "packages/drafter/src/ConversionContext.h",
        "packages/drafter/src/ElementInfoUtils.h",
        "packages/drafter/src/ElementComparator.h",

        "packages/drafter/src/SourceMapUtils.h",
        "packages/drafter/src/SourceMapUtils.cc",

        "packages/drafter/src/utils/Utf8.h",
        "packages/drafter/src/utils/Utils.h",
        "packages/drafter/src/utils/so/Value.h",
        "packages/drafter/src/utils/so/Value.cc",
        "packages/drafter/src/utils/so/JsonIo.h",
        "packages/drafter/src/utils/so/JsonIo.cc",
        "packages/drafter/src/utils/so/YamlIo.h",
        "packages/drafter/src/utils/so/YamlIo.cc",
        "packages/drafter/src/utils/log/Trivial.h",
        "packages/drafter/src/utils/log/Trivial.cc",

        # librefract parts - will be separated into other project
        "packages/drafter/src/refract/Utils.h",
        "packages/drafter/src/refract/Utils.cc",
        "packages/drafter/src/refract/InfoElements.h",
        "packages/drafter/src/refract/InfoElements.cc",
        "packages/drafter/src/refract/InfoElementsUtils.h",
        "packages/drafter/src/refract/ElementFwd.h",
        "packages/drafter/src/refract/ElementIfc.h",
        "packages/drafter/src/refract/Element.h",
        "packages/drafter/src/refract/Element.cc",
        "packages/drafter/src/refract/TypeQueryVisitor.h",
        "packages/drafter/src/refract/TypeQueryVisitor.cc",
        "packages/drafter/src/refract/VisitorUtils.h",
        "packages/drafter/src/refract/VisitorUtils.cc",
        "packages/drafter/src/refract/Visitor.h",

        "packages/drafter/src/refract/ComparableVisitor.h",
        "packages/drafter/src/refract/ComparableVisitor.cc",
        "packages/drafter/src/refract/IsExpandableVisitor.h",
        "packages/drafter/src/refract/IsExpandableVisitor.cc",
        "packages/drafter/src/refract/ExpandVisitor.h",
        "packages/drafter/src/refract/ExpandVisitor.cc",
        "packages/drafter/src/refract/PrintVisitor.h",
        "packages/drafter/src/refract/PrintVisitor.cc",
        "packages/drafter/src/refract/FilterVisitor.h",
        "packages/drafter/src/refract/JsonSchema.h",
        "packages/drafter/src/refract/JsonSchema.cc",
        "packages/drafter/src/refract/JsonValue.h",
        "packages/drafter/src/refract/JsonValue.cc",
        "packages/drafter/src/refract/JsonUtils.h",
        "packages/drafter/src/refract/JsonUtils.cc",
        "packages/drafter/src/refract/ElementUtils.h",
        "packages/drafter/src/refract/ElementUtils.cc",
        "packages/drafter/src/refract/ElementSize.h",
        "packages/drafter/src/refract/ElementSize.cc",
        "packages/drafter/src/refract/Cardinal.h",
        "packages/drafter/src/refract/SerializeSo.h",
        "packages/drafter/src/refract/SerializeSo.cc",

        "packages/drafter/src/refract/Registry.h",
        "packages/drafter/src/refract/Registry.cc",

        "packages/drafter/src/refract/Query.h",
        "packages/drafter/src/refract/Query.cc",
        "packages/drafter/src/refract/Iterate.h",

        "packages/drafter/src/refract/dsd/Utils.h",
        "packages/drafter/src/refract/dsd/Array.h",
        "packages/drafter/src/refract/dsd/Bool.h",
        "packages/drafter/src/refract/dsd/Enum.h",
        "packages/drafter/src/refract/dsd/Extend.h",
        "packages/drafter/src/refract/dsd/Holder.h",
        "packages/drafter/src/refract/dsd/Member.h",
        "packages/drafter/src/refract/dsd/Null.h",
        "packages/drafter/src/refract/dsd/Number.h",
        "packages/drafter/src/refract/dsd/Object.h",
        "packages/drafter/src/refract/dsd/Option.h",
        "packages/drafter/src/refract/dsd/Ref.h",
        "packages/drafter/src/refract/dsd/Select.h",
        "packages/drafter/src/refract/dsd/String.h",
        "packages/drafter/src/refract/dsd/Traits.h",

        "packages/drafter/src/refract/dsd/Array.cc",
        "packages/drafter/src/refract/dsd/Bool.cc",
        "packages/drafter/src/refract/dsd/Enum.cc",
        "packages/drafter/src/refract/dsd/Extend.cc",
        "packages/drafter/src/refract/dsd/Holder.cc",
        "packages/drafter/src/refract/dsd/Member.cc",
        "packages/drafter/src/refract/dsd/Null.cc",
        "packages/drafter/src/refract/dsd/Number.cc",
        "packages/drafter/src/refract/dsd/Object.cc",
        "packages/drafter/src/refract/dsd/Option.cc",
        "packages/drafter/src/refract/dsd/Ref.cc",
        "packages/drafter/src/refract/dsd/Select.cc",
        "packages/drafter/src/refract/dsd/String.cc",

        "packages/drafter/src/backend/MediaTypeS11n.cc",
        "packages/drafter/src/backend/MediaTypeS11n.h",
        "packages/drafter/src/backend/Backend.h",
      ],
      "dependencies": [
        "libapib-parser",
        "libapib",
      ],
    },

# TEST-LIBDRAFTER
    {
      'target_name': 'test-libdrafter',
      'type': 'executable',
      'include_dirs': [
        'packages/Catch/single_include',
        'packages/dtl/dtl',
        'packages/drafter/src/refract',
      ],
      'defines': [ 'DRAFTER_TEST_FIXTURES="packages/drafter/test/fixtures/"' ],
      'sources': [
        "packages/drafter/test/draftertest.cc",
        "packages/drafter/test/test-drafter.cc",
        "packages/drafter/test/test-RefractDataStructureTest.cc",
        "packages/drafter/test/test-RefractAPITest.cc",
        "packages/drafter/test/test-RefractParseResultTest.cc",
        "packages/drafter/test/test-RenderTest.cc",
        "packages/drafter/test/test-RefractSourceMapTest.cc",
        "packages/drafter/test/test-SchemaTest.cc",
        "packages/drafter/test/test-CircularReferenceTest.cc",
        "packages/drafter/test/test-ApplyVisitorTest.cc",
        "packages/drafter/test/test-ExtendElementTest.cc",
        "packages/drafter/test/test-ElementFactoryTest.cc",
        "packages/drafter/test/test-OneOfTest.cc",
        "packages/drafter/test/test-SyntaxIssuesTest.cc",
        "packages/drafter/test/test-ElementDataTest.cc",
        "packages/drafter/test/test-Serialize.cc",

        "packages/drafter/test/utils/test-Utf8.cc",
        "packages/drafter/test/utils/so/test-JsonIo.cc",
        "packages/drafter/test/utils/so/test-YamlIo.cc",

        "packages/drafter/test/refract/test-Utils.cc",
        "packages/drafter/test/refract/test-JsonSchema.cc",
        "packages/drafter/test/refract/test-JsonValue.cc",
        "packages/drafter/test/refract/test-ElementSize.cc",
        "packages/drafter/test/refract/test-Cardinal.cc",

        "packages/drafter/test/refract/dsd/test-Array.cc",
        "packages/drafter/test/refract/dsd/test-Bool.cc",
        "packages/drafter/test/refract/dsd/test-Enum.cc",
        "packages/drafter/test/refract/dsd/test-Extend.cc",
        "packages/drafter/test/refract/dsd/test-Holder.cc",
        "packages/drafter/test/refract/dsd/test-Member.cc",
        "packages/drafter/test/refract/dsd/test-Null.cc",
        "packages/drafter/test/refract/dsd/test-Number.cc",
        "packages/drafter/test/refract/dsd/test-Object.cc",
        "packages/drafter/test/refract/dsd/test-Option.cc",
        "packages/drafter/test/refract/dsd/test-Ref.cc",
        "packages/drafter/test/refract/dsd/test-Select.cc",
        "packages/drafter/test/refract/dsd/test-String.cc",

        "packages/drafter/test/refract/dsd/test-Element.cc",
        "packages/drafter/test/refract/dsd/test-InfoElements.cc",
        "packages/drafter/test/refract/test-InfoElementsUtils.cc",

        "packages/drafter/test/test-ElementInfoUtils.cc",
        "packages/drafter/test/test-ElementComparator.cc",
        "packages/drafter/test/test-VisitorUtils.cc",
        "packages/drafter/test/test-sourceMapToLineColumn.cc",

        "packages/drafter/test/backend/test-MediaTypeS11.cc",
      ],
      'dependencies': [
        "libdrafter",
      ],
      'conditions': [
         [ 'OS=="win"', { 'defines' : [ 'WIN' ] } ]
      ],
    },

# DRAFTER
    {
      "target_name": "drafter",
      "type": "executable",
      "conditions" : [
        [ 'libdrafter_type=="static_library"', { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      "defines": ["LOGGING"],
      "sources": [
        "packages/drafter/src/main.cc",
        "packages/drafter/src/config.cc",
        "packages/drafter/src/config.h",
        "packages/drafter/src/reporting.cc",
        "packages/drafter/src/reporting.h",
      ],
      "include_dirs": [
        "packages/cmdline",
      ],
      "dependencies": [
        "libdrafter",
      ],
    },

# DRAFTER C-API TEST
    {
      "target_name": "test-capi",
      "type": "executable",
      "conditions" : [
        [ 'libdrafter_type=="static_library"', { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      "sources": [
        "packages/drafter/test/test-CAPI.c",
        "packages/drafter/test/ctesting.h",
        "packages/drafter/test/ctesting.c"
      ],
      "dependencies": [
        "libdrafter",
      ],
    },
  ],
}
