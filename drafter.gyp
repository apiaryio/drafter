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

# LIBMARKDOWNPARSER
    {
      'target_name': 'libmarkdownparser',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/markdown-parser/src',
        ],
      },
      'export_dependent_settings': [
        'libsundown'
      ],
      'sources': [
        'packages/markdown-parser/src/ByteBuffer.cc',
        'packages/markdown-parser/src/ByteBuffer.h',
        'packages/markdown-parser/src/MarkdownNode.cc',
        'packages/markdown-parser/src/MarkdownNode.h',
        'packages/markdown-parser/src/MarkdownParser.cc',
        'packages/markdown-parser/src/MarkdownParser.h'
      ],
      'dependencies': [
        'libsundown'
      ]
    },

# TEST-LIBMARKDOWNPARSER
    {
      'target_name': 'test-libmarkdownparser',
      'type': 'executable',
      'include_dirs': [
        'test/vendor/Catch/single_include',
      ],
      'sources': [
        'markdown-parser/test/test-ByteBuffer.cc',
        'markdown-parser/test/test-MarkdownParser.cc',
        'markdown-parser/test/test-libmarkdownparser.cc'
      ],
      'dependencies': [
        'libmarkdownparser',
      ]
    },

# LIBSNOWCRASH
    {
      'target_name': 'libsnowcrash',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'packages/snowcrash/src',
          'packages/variant/include',
          'packages/boost',
        ],
      },
      'include_dirs' : [
        'packages/boost',
        'packages/variant/include',
      ],
      'export_dependent_settings': [
        'libmarkdownparser'
      ],
      'sources': [
        'packages/snowcrash/src/HTTP.cc',
        'packages/snowcrash/src/HTTP.h',
        'packages/snowcrash/src/MSON.cc',
        'packages/snowcrash/src/MSONOneOfParser.cc',
        'packages/snowcrash/src/MSONSourcemap.cc',
        'packages/snowcrash/src/MSONTypeSectionParser.cc',
        'packages/snowcrash/src/MSONValueMemberParser.cc',
        'packages/snowcrash/src/Blueprint.cc',
        'packages/snowcrash/src/BlueprintSourcemap.cc',
        'packages/snowcrash/src/Section.cc',
        'packages/snowcrash/src/Section.h',
        'packages/snowcrash/src/Signature.cc',
        'packages/snowcrash/src/Signature.h',
        'packages/snowcrash/src/snowcrash.cc',
        'packages/snowcrash/src/snowcrash.h',
        'packages/snowcrash/src/UriTemplateParser.cc',
        'packages/snowcrash/src/UriTemplateParser.h',
        'packages/snowcrash/src/PayloadParser.h',
        'packages/snowcrash/src/SectionParserData.h',
        'packages/snowcrash/src/ActionParser.h',
        'packages/snowcrash/src/AssetParser.h',
        'packages/snowcrash/src/AttributesParser.h',
        'packages/snowcrash/src/Blueprint.h',
        'packages/snowcrash/src/BlueprintParser.h',
        'packages/snowcrash/src/BlueprintSourcemap.h',
        'packages/snowcrash/src/BlueprintUtility.h',
        'packages/snowcrash/src/CodeBlockUtility.h',
        'packages/snowcrash/src/DataStructureGroupParser.h',
        'packages/snowcrash/src/HeadersParser.h',
        'packages/snowcrash/src/HeadersParser.cc',
        'packages/snowcrash/src/ModelTable.h',
        'packages/snowcrash/src/MSON.h',
        'packages/snowcrash/src/MSONSourcemap.h',
        'packages/snowcrash/src/MSONMixinParser.h',
        'packages/snowcrash/src/MSONNamedTypeParser.h',
        'packages/snowcrash/src/MSONOneOfParser.h',
        'packages/snowcrash/src/MSONParameterParser.h',
        'packages/snowcrash/src/MSONPropertyMemberParser.h',
        'packages/snowcrash/src/MSONTypeSectionParser.h',
        'packages/snowcrash/src/MSONUtility.h',
        'packages/snowcrash/src/MSONValueMemberParser.h',
        'packages/snowcrash/src/ParameterParser.h',
        'packages/snowcrash/src/ParametersParser.h',
        'packages/snowcrash/src/Platform.h',
        'packages/snowcrash/src/RegexMatch.h',
        'packages/snowcrash/src/RelationParser.h',
        'packages/snowcrash/src/ResourceGroupParser.h',
        'packages/snowcrash/src/ResourceParser.h',
        'packages/snowcrash/src/SectionParser.h',
        'packages/snowcrash/src/SectionProcessor.h',
        'packages/snowcrash/src/SignatureSectionProcessor.h',
        'packages/snowcrash/src/SourceAnnotation.h',
        'packages/snowcrash/src/StringUtility.h',
        'packages/snowcrash/src/ValuesParser.h',
      ],
      'conditions': [
        [ 'OS=="win"',
          { 'sources': [ 'packages/snowcrash/src/win/RegexMatch.cc' ] },
          { 'sources': [ 'packages/snowcrash/src/posix/RegexMatch.cc' ] } # OS != Windows
        ]
      ],
      'dependencies': [
        'libmarkdownparser',
      ]
    },

# TEST-LIBSNOWCRASH
    {
      'target_name': 'test-libsnowcrash',
      'type': 'executable',
      'include_dirs': [
        'test/vendor/Catch/single_include',
      ],
      'sources': [
        'packages/snowcrash/test/test-ActionParser.cc',
        'packages/snowcrash/test/test-AssetParser.cc',
        'packages/snowcrash/test/test-AttributesParser.cc',
        'packages/snowcrash/test/test-Blueprint.cc',
        'packages/snowcrash/test/test-BlueprintParser.cc',
        'packages/snowcrash/test/test-BlueprintUtility.cc',
        'packages/snowcrash/test/test-DataStructureGroupParser.cc',
        'packages/snowcrash/test/test-HeadersParser.cc',
        'packages/snowcrash/test/test-Indentation.cc',
        'packages/snowcrash/test/test-ModelTable.cc',
        'packages/snowcrash/test/test-MSONMixinParser.cc',
        'packages/snowcrash/test/test-MSONNamedTypeParser.cc',
        'packages/snowcrash/test/test-MSONOneOfParser.cc',
        'packages/snowcrash/test/test-MSONParameterParser.cc',
        'packages/snowcrash/test/test-MSONPropertyMemberParser.cc',
        'packages/snowcrash/test/test-MSONTypeSectionParser.cc',
        'packages/snowcrash/test/test-MSONUtility.cc',
        'packages/snowcrash/test/test-MSONValueMemberParser.cc',
        'packages/snowcrash/test/test-ParameterParser.cc',
        'packages/snowcrash/test/test-ParametersParser.cc',
        'packages/snowcrash/test/test-PayloadParser.cc',
        'packages/snowcrash/test/test-RegexMatch.cc',
        'packages/snowcrash/test/test-RelationParser.cc',
        'packages/snowcrash/test/test-ResourceParser.cc',
        'packages/snowcrash/test/test-ResourceGroupParser.cc',
        'packages/snowcrash/test/test-SectionParser.cc',
        'packages/snowcrash/test/test-Signature.cc',
        'packages/snowcrash/test/test-StringUtility.cc',
        'packages/snowcrash/test/test-SymbolIdentifier.cc',
        'packages/snowcrash/test/test-UriTemplateParser.cc',
        'packages/snowcrash/test/test-ValuesParser.cc',
        'packages/snowcrash/test/test-Warnings.cc',
        'packages/snowcrash/test/test-snowcrash.cc'
      ],
      'dependencies': [
        'libsnowcrash',
      ]
    },

# PERF-LIBSNOWCRASH
    {
      'target_name': 'perf-libsnowcrash',
      'type': 'executable',
      'sources': [
        'packages/snowcrash/test/performance/perf-snowcrash.cc'
      ],
      'dependencies': [
        'libsnowcrash',
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
          'src',
          'packages/boost',
          'packages/variant/include',
          'packages/PEGTL/include',
        ],
      },
      'include_dirs' : [
        'packages/boost',
        'packages/variant/include',
        'packages/PEGTL/include',
      ],
      'export_dependent_settings': [
        'libsnowcrash',
      ],
      "sources": [
        "src/drafter.h",
        "src/options.h",
        "src/drafter.cc",
        "src/options.cc",
        "src/stream.h",
        "src/Version.h",

        "src/NodeInfo.h",
        "src/Serialize.h",
        "src/Serialize.cc",
        "src/SerializeKey.h",
        "src/SerializeKey.cc",
        "src/SerializeResult.h",
        "src/SerializeResult.cc",
        "src/RefractAPI.h",
        "src/RefractAPI.cc",
        "src/MsonTypeSectionToApie.h",
        "src/MsonTypeSectionToApie.cc",
        "src/MsonMemberToApie.h",
        "src/MsonMemberToApie.cc",
        "src/MsonOneOfSectionToApie.h",
        "src/MsonOneOfSectionToApie.cc",
        "src/RefractDataStructure.h",
        "src/RefractDataStructure.cc",
        "src/RefractSourceMap.h",
        "src/RefractSourceMap.cc",
        "src/Render.h",
        "src/Render.cc",
        "src/NamedTypesRegistry.cc",
        "src/NamedTypesRegistry.h",
        "src/RefractElementFactory.h",
        "src/RefractElementFactory.cc",
        "src/ConversionContext.cc",
        "src/ConversionContext.h",
        "src/ElementInfoUtils.h",
        "src/ElementComparator.h",
        "src/ContentTypeMatcher.h",
        "src/ContentTypeMatcher.cc",

        "src/SourceMapUtils.h",
        "src/SourceMapUtils.cc",

        "src/utils/Utf8.h",
        "src/utils/Utils.h",
        "src/utils/so/Value.h",
        "src/utils/so/Value.cc",
        "src/utils/so/JsonIo.h",
        "src/utils/so/JsonIo.cc",
        "src/utils/so/YamlIo.h",
        "src/utils/so/YamlIo.cc",
        "src/utils/log/Trivial.h",
        "src/utils/log/Trivial.cc",

        # librefract parts - will be separated into other project
        "src/refract/Utils.h",
        "src/refract/Utils.cc",
        "src/refract/InfoElements.h",
        "src/refract/InfoElements.cc",
        "src/refract/InfoElementsUtils.h",
        "src/refract/ElementFwd.h",
        "src/refract/ElementIfc.h",
        "src/refract/Element.h",
        "src/refract/Element.cc",
        "src/refract/TypeQueryVisitor.h",
        "src/refract/TypeQueryVisitor.cc",
        "src/refract/VisitorUtils.h",
        "src/refract/VisitorUtils.cc",
        "src/refract/Visitor.h",

        "src/refract/ComparableVisitor.h",
        "src/refract/ComparableVisitor.cc",
        "src/refract/IsExpandableVisitor.h",
        "src/refract/IsExpandableVisitor.cc",
        "src/refract/ExpandVisitor.h",
        "src/refract/ExpandVisitor.cc",
        "src/refract/PrintVisitor.h",
        "src/refract/PrintVisitor.cc",
        "src/refract/FilterVisitor.h",
        "src/refract/JsonSchema.h",
        "src/refract/JsonSchema.cc",
        "src/refract/JsonValue.h",
        "src/refract/JsonValue.cc",
        "src/refract/JsonUtils.h",
        "src/refract/JsonUtils.cc",
        "src/refract/ElementUtils.h",
        "src/refract/ElementUtils.cc",
        "src/refract/ElementSize.h",
        "src/refract/ElementSize.cc",
        "src/refract/Cardinal.h",
        "src/refract/SerializeSo.h",
        "src/refract/SerializeSo.cc",

        "src/refract/Registry.h",
        "src/refract/Registry.cc",

        "src/refract/Query.h",
        "src/refract/Query.cc",
        "src/refract/Iterate.h",

        "src/refract/dsd/Utils.h",
        "src/refract/dsd/Array.h",
        "src/refract/dsd/Bool.h",
        "src/refract/dsd/Enum.h",
        "src/refract/dsd/Extend.h",
        "src/refract/dsd/Holder.h",
        "src/refract/dsd/Member.h",
        "src/refract/dsd/Null.h",
        "src/refract/dsd/Number.h",
        "src/refract/dsd/Object.h",
        "src/refract/dsd/Option.h",
        "src/refract/dsd/Ref.h",
        "src/refract/dsd/Select.h",
        "src/refract/dsd/String.h",
        "src/refract/dsd/Traits.h",

        "src/refract/dsd/Array.cc",
        "src/refract/dsd/Bool.cc",
        "src/refract/dsd/Enum.cc",
        "src/refract/dsd/Extend.cc",
        "src/refract/dsd/Holder.cc",
        "src/refract/dsd/Member.cc",
        "src/refract/dsd/Null.cc",
        "src/refract/dsd/Number.cc",
        "src/refract/dsd/Object.cc",
        "src/refract/dsd/Option.cc",
        "src/refract/dsd/Ref.cc",
        "src/refract/dsd/Select.cc",
        "src/refract/dsd/String.cc",

        "src/parser/Mediatype.h",
        "src/parser/Mediatype.cc",
        "src/parser/recover.h",
        "src/parser/UriTemplateParser.h",

        "src/backend/MediaTypeS11n.cc",
        "src/backend/MediaTypeS11n.h",
        "src/backend/Backend.h",
      ],
      "dependencies": [
        "libsnowcrash",
      ],
    },

# TEST-LIBDRAFTER
    {
      'target_name': 'test-libdrafter',
      'type': 'executable',
      'include_dirs': [
        'test/vendor/Catch/single_include',
        'test/vendor/dtl/dtl',
        'src/refract',
      ],
      'sources': [
        "test/draftertest.cc",
        "test/test-drafter.cc",
        "test/test-RefractDataStructureTest.cc",
        "test/test-RefractAPITest.cc",
        "test/test-RefractParseResultTest.cc",
        "test/test-RenderTest.cc",
        "test/test-RefractSourceMapTest.cc",
        "test/test-SchemaTest.cc",
        "test/test-CircularReferenceTest.cc",
        "test/test-ApplyVisitorTest.cc",
        "test/test-ExtendElementTest.cc",
        "test/test-ElementFactoryTest.cc",
        "test/test-OneOfTest.cc",
        "test/test-SyntaxIssuesTest.cc",
        "test/test-ElementDataTest.cc",
        "test/test-Serialize.cc",
        "test/test-ContentTypeMatcher.cc",

        "test/utils/test-Utf8.cc",
        "test/utils/so/test-JsonIo.cc",
        "test/utils/so/test-YamlIo.cc",

        "test/refract/test-Utils.cc",
        "test/refract/test-JsonSchema.cc",
        "test/refract/test-JsonValue.cc",
        "test/refract/test-ElementSize.cc",
        "test/refract/test-Cardinal.cc",

        "test/refract/dsd/test-Array.cc",
        "test/refract/dsd/test-Bool.cc",
        "test/refract/dsd/test-Enum.cc",
        "test/refract/dsd/test-Extend.cc",
        "test/refract/dsd/test-Holder.cc",
        "test/refract/dsd/test-Member.cc",
        "test/refract/dsd/test-Null.cc",
        "test/refract/dsd/test-Number.cc",
        "test/refract/dsd/test-Object.cc",
        "test/refract/dsd/test-Option.cc",
        "test/refract/dsd/test-Ref.cc",
        "test/refract/dsd/test-Select.cc",
        "test/refract/dsd/test-String.cc",

        "test/refract/dsd/test-Element.cc",
        "test/refract/dsd/test-InfoElements.cc",
        "test/refract/test-InfoElementsUtils.cc",

        "test/test-ElementInfoUtils.cc",
        "test/test-ElementComparator.cc",
        "test/test-VisitorUtils.cc",
        "test/test-sourceMapToLineColumn.cc",

        "test/parser/test-MediaType.cc",
        "test/parser/test-Recover.cc",
        "test/parser/test-UriTemplate.cc",

        "test/backend/test-MediaTypeS11.cc",
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
        "src/main.cc",
        "src/config.cc",
        "src/config.h",
        "src/reporting.cc",
        "src/reporting.h",
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
        "test/test-CAPI.c",
        "test/ctesting.h",
        "test/ctesting.c"
      ],
      "dependencies": [
        "libdrafter",
      ],
    },
  ],
}
