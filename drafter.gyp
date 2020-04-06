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
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html'
      ],
      'direct_dependent_settings' : {
        'include_dirs': [
          'ext/snowcrash/ext/markdown-parser/ext/sundown/src',
        ],
      },
      'sources': [
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/autolink.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/autolink.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/buffer.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/buffer.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/html_blocks.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/markdown.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/markdown.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/src_map.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/src_map.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/stack.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/src/stack.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/houdini.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/houdini_href_e.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/houdini_html_e.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/html.c',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/html.h',
        'ext/snowcrash/ext/markdown-parser/ext/sundown/html/html_smartypants.c'
      ]
    },

# LIBMARKDOWNPARSER
    {
      'target_name': 'libmarkdownparser',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'ext/snowcrash/ext/markdown-parser/src',
        ],
      },
      'export_dependent_settings': [
        'libsundown'
      ],
      'sources': [
        'ext/snowcrash/ext/markdown-parser/src/ByteBuffer.cc',
        'ext/snowcrash/ext/markdown-parser/src/ByteBuffer.h',
        'ext/snowcrash/ext/markdown-parser/src/MarkdownNode.cc',
        'ext/snowcrash/ext/markdown-parser/src/MarkdownNode.h',
        'ext/snowcrash/ext/markdown-parser/src/MarkdownParser.cc',
        'ext/snowcrash/ext/markdown-parser/src/MarkdownParser.h'
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
        'ext/snowcrash/ext/markdown-parser/test/test-ByteBuffer.cc',
        'ext/snowcrash/ext/markdown-parser/test/test-MarkdownParser.cc',
        'ext/snowcrash/ext/markdown-parser/test/test-libmarkdownparser.cc'
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
          'ext/snowcrash/src',
          'ext/variant/include',
          'ext/boost',
        ],
      },
      'include_dirs' : [
        'ext/boost',
        'ext/variant/include',
      ],
      'export_dependent_settings': [
        'libmarkdownparser'
      ],
      'sources': [
        'ext/snowcrash/src/HTTP.cc',
        'ext/snowcrash/src/HTTP.h',
        'ext/snowcrash/src/MSON.cc',
        'ext/snowcrash/src/MSONOneOfParser.cc',
        'ext/snowcrash/src/MSONSourcemap.cc',
        'ext/snowcrash/src/MSONTypeSectionParser.cc',
        'ext/snowcrash/src/MSONValueMemberParser.cc',
        'ext/snowcrash/src/Blueprint.cc',
        'ext/snowcrash/src/BlueprintSourcemap.cc',
        'ext/snowcrash/src/Section.cc',
        'ext/snowcrash/src/Section.h',
        'ext/snowcrash/src/Signature.cc',
        'ext/snowcrash/src/Signature.h',
        'ext/snowcrash/src/snowcrash.cc',
        'ext/snowcrash/src/snowcrash.h',
        'ext/snowcrash/src/UriTemplateParser.cc',
        'ext/snowcrash/src/UriTemplateParser.h',
        'ext/snowcrash/src/PayloadParser.h',
        'ext/snowcrash/src/SectionParserData.h',
        'ext/snowcrash/src/ActionParser.h',
        'ext/snowcrash/src/AssetParser.h',
        'ext/snowcrash/src/AttributesParser.h',
        'ext/snowcrash/src/Blueprint.h',
        'ext/snowcrash/src/BlueprintParser.h',
        'ext/snowcrash/src/BlueprintSourcemap.h',
        'ext/snowcrash/src/BlueprintUtility.h',
        'ext/snowcrash/src/CodeBlockUtility.h',
        'ext/snowcrash/src/DataStructureGroupParser.h',
        'ext/snowcrash/src/HeadersParser.h',
        'ext/snowcrash/src/HeadersParser.cc',
        'ext/snowcrash/src/ModelTable.h',
        'ext/snowcrash/src/MSON.h',
        'ext/snowcrash/src/MSONSourcemap.h',
        'ext/snowcrash/src/MSONMixinParser.h',
        'ext/snowcrash/src/MSONNamedTypeParser.h',
        'ext/snowcrash/src/MSONOneOfParser.h',
        'ext/snowcrash/src/MSONParameterParser.h',
        'ext/snowcrash/src/MSONPropertyMemberParser.h',
        'ext/snowcrash/src/MSONTypeSectionParser.h',
        'ext/snowcrash/src/MSONUtility.h',
        'ext/snowcrash/src/MSONValueMemberParser.h',
        'ext/snowcrash/src/ParameterParser.h',
        'ext/snowcrash/src/ParametersParser.h',
        'ext/snowcrash/src/Platform.h',
        'ext/snowcrash/src/RegexMatch.h',
        'ext/snowcrash/src/RelationParser.h',
        'ext/snowcrash/src/ResourceGroupParser.h',
        'ext/snowcrash/src/ResourceParser.h',
        'ext/snowcrash/src/SectionParser.h',
        'ext/snowcrash/src/SectionProcessor.h',
        'ext/snowcrash/src/SignatureSectionProcessor.h',
        'ext/snowcrash/src/SourceAnnotation.h',
        'ext/snowcrash/src/StringUtility.h',
        'ext/snowcrash/src/ValuesParser.h',
      ],
      'conditions': [
        [ 'OS=="win"',
          { 'sources': [ 'ext/snowcrash/src/win/RegexMatch.cc' ] },
          { 'sources': [ 'ext/snowcrash/src/posix/RegexMatch.cc' ] } # OS != Windows
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
        'ext/snowcrash/test/test-ActionParser.cc',
        'ext/snowcrash/test/test-AssetParser.cc',
        'ext/snowcrash/test/test-AttributesParser.cc',
        'ext/snowcrash/test/test-Blueprint.cc',
        'ext/snowcrash/test/test-BlueprintParser.cc',
        'ext/snowcrash/test/test-BlueprintUtility.cc',
        'ext/snowcrash/test/test-DataStructureGroupParser.cc',
        'ext/snowcrash/test/test-HeadersParser.cc',
        'ext/snowcrash/test/test-Indentation.cc',
        'ext/snowcrash/test/test-ModelTable.cc',
        'ext/snowcrash/test/test-MSONMixinParser.cc',
        'ext/snowcrash/test/test-MSONNamedTypeParser.cc',
        'ext/snowcrash/test/test-MSONOneOfParser.cc',
        'ext/snowcrash/test/test-MSONParameterParser.cc',
        'ext/snowcrash/test/test-MSONPropertyMemberParser.cc',
        'ext/snowcrash/test/test-MSONTypeSectionParser.cc',
        'ext/snowcrash/test/test-MSONUtility.cc',
        'ext/snowcrash/test/test-MSONValueMemberParser.cc',
        'ext/snowcrash/test/test-ParameterParser.cc',
        'ext/snowcrash/test/test-ParametersParser.cc',
        'ext/snowcrash/test/test-PayloadParser.cc',
        'ext/snowcrash/test/test-RegexMatch.cc',
        'ext/snowcrash/test/test-RelationParser.cc',
        'ext/snowcrash/test/test-ResourceParser.cc',
        'ext/snowcrash/test/test-ResourceGroupParser.cc',
        'ext/snowcrash/test/test-SectionParser.cc',
        'ext/snowcrash/test/test-Signature.cc',
        'ext/snowcrash/test/test-StringUtility.cc',
        'ext/snowcrash/test/test-SymbolIdentifier.cc',
        'ext/snowcrash/test/test-UriTemplateParser.cc',
        'ext/snowcrash/test/test-ValuesParser.cc',
        'ext/snowcrash/test/test-Warnings.cc',
        'ext/snowcrash/test/test-snowcrash.cc'
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
        'ext/snowcrash/test/performance/perf-snowcrash.cc'
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
          'ext/boost',
          'ext/variant/include',
          'ext/PEGTL/include',
        ],
      },
      'include_dirs' : [
        'ext/boost',
        'ext/variant/include',
        'ext/PEGTL/include',
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
        "ext/cmdline",
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
