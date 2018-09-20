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
        'ext/sundown/src',
        'ext/sundown/html'
      ],
      'direct_dependent_settings' : {
        'include_dirs': [
          'ext/sundown/src',
        ],
      },
      'sources': [
        'ext/sundown/src/autolink.c',
        'ext/sundown/src/autolink.h',
        'ext/sundown/src/buffer.c',
        'ext/sundown/src/buffer.h',
        'ext/sundown/src/html_blocks.h',
        'ext/sundown/src/markdown.c',
        'ext/sundown/src/markdown.h',
        'ext/sundown/src/src_map.c',
        'ext/sundown/src/src_map.h',
        'ext/sundown/src/stack.c',
        'ext/sundown/src/stack.h',
        'ext/sundown/html/houdini.h',
        'ext/sundown/html/houdini_href_e.c',
        'ext/sundown/html/houdini_html_e.c',
        'ext/sundown/html/html.c',
        'ext/sundown/html/html.h',
        'ext/sundown/html/html_smartypants.c'
      ]
    },

# LIBMARKDOWNPARSER
    {
      'target_name': 'libmarkdownparser',
      'type': 'static_library',
      'direct_dependent_settings' : {
        'include_dirs': [
          'markdown-parser/src',
        ],
      },
      'export_dependent_settings': [
        'libsundown'
      ],
      'sources': [
        'markdown-parser/src/ByteBuffer.cc',
        'markdown-parser/src/ByteBuffer.h',
        'markdown-parser/src/MarkdownNode.cc',
        'markdown-parser/src/MarkdownNode.h',
        'markdown-parser/src/MarkdownParser.cc',
        'markdown-parser/src/MarkdownParser.h'
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
        'ext/Catch/single_include',
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
          'snowcrash/src',
        ],
      },
      'export_dependent_settings': [
        'libmarkdownparser'
      ],
      'sources': [
        'snowcrash/src/HTTP.cc',
        'snowcrash/src/HTTP.h',
        'snowcrash/src/MSON.cc',
        'snowcrash/src/MSONOneOfParser.cc',
        'snowcrash/src/MSONSourcemap.cc',
        'snowcrash/src/MSONTypeSectionParser.cc',
        'snowcrash/src/MSONValueMemberParser.cc',
        'snowcrash/src/Blueprint.cc',
        'snowcrash/src/BlueprintSourcemap.cc',
        'snowcrash/src/Section.cc',
        'snowcrash/src/Section.h',
        'snowcrash/src/Signature.cc',
        'snowcrash/src/Signature.h',
        'snowcrash/src/snowcrash.cc',
        'snowcrash/src/snowcrash.h',
        'snowcrash/src/UriTemplateParser.cc',
        'snowcrash/src/UriTemplateParser.h',
        'snowcrash/src/PayloadParser.h',
        'snowcrash/src/SectionParserData.h',
        'snowcrash/src/ActionParser.h',
        'snowcrash/src/AssetParser.h',
        'snowcrash/src/AttributesParser.h',
        'snowcrash/src/Blueprint.h',
        'snowcrash/src/BlueprintParser.h',
        'snowcrash/src/BlueprintSourcemap.h',
        'snowcrash/src/BlueprintUtility.h',
        'snowcrash/src/CodeBlockUtility.h',
        'snowcrash/src/DataStructureGroupParser.h',
        'snowcrash/src/HeadersParser.h',
        'snowcrash/src/HeadersParser.cc',
        'snowcrash/src/ModelTable.h',
        'snowcrash/src/MSON.h',
        'snowcrash/src/MSONSourcemap.h',
        'snowcrash/src/MSONMixinParser.h',
        'snowcrash/src/MSONNamedTypeParser.h',
        'snowcrash/src/MSONOneOfParser.h',
        'snowcrash/src/MSONParameterParser.h',
        'snowcrash/src/MSONPropertyMemberParser.h',
        'snowcrash/src/MSONTypeSectionParser.h',
        'snowcrash/src/MSONUtility.h',
        'snowcrash/src/MSONValueMemberParser.h',
        'snowcrash/src/ParameterParser.h',
        'snowcrash/src/ParametersParser.h',
        'snowcrash/src/Platform.h',
        'snowcrash/src/RegexMatch.h',
        'snowcrash/src/RelationParser.h',
        'snowcrash/src/ResourceGroupParser.h',
        'snowcrash/src/ResourceParser.h',
        'snowcrash/src/SectionParser.h',
        'snowcrash/src/SectionProcessor.h',
        'snowcrash/src/SignatureSectionProcessor.h',
        'snowcrash/src/SourceAnnotation.h',
        'snowcrash/src/StringUtility.h',
        'snowcrash/src/ValuesParser.h',
      ],
      'conditions': [
        [ 'OS=="win"',
          { 'sources': [ 'snowcrash/src/win/RegexMatch.cc' ] },
          { 'sources': [ 'snowcrash/src/posix/RegexMatch.cc' ] } # OS != Windows
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
        'ext/Catch/single_include',
      ],
      'sources': [
        'snowcrash/test/test-ActionParser.cc',
        'snowcrash/test/test-AssetParser.cc',
        'snowcrash/test/test-AttributesParser.cc',
        'snowcrash/test/test-Blueprint.cc',
        'snowcrash/test/test-BlueprintParser.cc',
        'snowcrash/test/test-BlueprintUtility.cc',
        'snowcrash/test/test-DataStructureGroupParser.cc',
        'snowcrash/test/test-HeadersParser.cc',
        'snowcrash/test/test-Indentation.cc',
        'snowcrash/test/test-ModelTable.cc',
        'snowcrash/test/test-MSONMixinParser.cc',
        'snowcrash/test/test-MSONNamedTypeParser.cc',
        'snowcrash/test/test-MSONOneOfParser.cc',
        'snowcrash/test/test-MSONParameterParser.cc',
        'snowcrash/test/test-MSONPropertyMemberParser.cc',
        'snowcrash/test/test-MSONTypeSectionParser.cc',
        'snowcrash/test/test-MSONUtility.cc',
        'snowcrash/test/test-MSONValueMemberParser.cc',
        'snowcrash/test/test-ParameterParser.cc',
        'snowcrash/test/test-ParametersParser.cc',
        'snowcrash/test/test-PayloadParser.cc',
        'snowcrash/test/test-RegexMatch.cc',
        'snowcrash/test/test-RelationParser.cc',
        'snowcrash/test/test-ResourceParser.cc',
        'snowcrash/test/test-ResourceGroupParser.cc',
        'snowcrash/test/test-SectionParser.cc',
        'snowcrash/test/test-Signature.cc',
        'snowcrash/test/test-StringUtility.cc',
        'snowcrash/test/test-SymbolIdentifier.cc',
        'snowcrash/test/test-UriTemplateParser.cc',
        'snowcrash/test/test-ValuesParser.cc',
        'snowcrash/test/test-Warnings.cc',
        'snowcrash/test/test-snowcrash.cc'
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
        'snowcrash/test/performance/perf-snowcrash.cc'
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
          'ext/boost-container/include',
          'ext/boost-config/include',
          'ext/boost-move/include',
          'ext/boost-core/include',
          'ext/boost-assert/include',
          'ext/boost-static_assert/include',
          'ext/boost-intrusive/include',
        ],
      },
      'include_dirs' : [
        'ext/boost-container/include',
        'ext/boost-config/include',
        'ext/boost-move/include',
        'ext/boost-core/include',
        'ext/boost-assert/include',
        'ext/boost-static_assert/include',
        'ext/boost-intrusive/include',
      ],
      'export_dependent_settings': [
        'libsnowcrash',
      ],
      "sources": [
        "drafter/src/drafter.h",
        "drafter/src/drafter.cc",
        "drafter/src/stream.h",
        "drafter/src/Version.h",

        "drafter/src/NodeInfo.h",
        "drafter/src/Serialize.h",
        "drafter/src/Serialize.cc",
        "drafter/src/SerializeResult.h",
        "drafter/src/SerializeResult.cc",
        "drafter/src/RefractAPI.h",
        "drafter/src/RefractAPI.cc",
        "drafter/src/RefractDataStructure.h",
        "drafter/src/RefractDataStructure.cc",
        "drafter/src/RefractSourceMap.h",
        "drafter/src/RefractSourceMap.cc",
        "drafter/src/Render.h",
        "drafter/src/Render.cc",
        "drafter/src/NamedTypesRegistry.cc",
        "drafter/src/NamedTypesRegistry.h",
        "drafter/src/RefractElementFactory.h",
        "drafter/src/RefractElementFactory.cc",
        "drafter/src/ConversionContext.cc",
        "drafter/src/ConversionContext.h",
        "drafter/src/ElementInfoUtils.h"
        "drafter/src/ElementComparator.h"

        "drafter/src/utils/Variant.h",
        "drafter/src/utils/Utf8.h",
        "drafter/src/utils/Utils.h",
        "drafter/src/utils/so/Value.h",
        "drafter/src/utils/so/Value.cc",
        "drafter/src/utils/so/JsonIo.h",
        "drafter/src/utils/so/JsonIo.cc",
        "drafter/src/utils/so/YamlIo.h",
        "drafter/src/utils/so/YamlIo.cc",
        "drafter/src/utils/log/Trivial.h",
        "drafter/src/utils/log/Trivial.cc",

        # librefract parts - will be separated into other project
        "drafter/src/refract/Utils.h",
        "drafter/src/refract/Utils.cc",
        "drafter/src/refract/InfoElements.h",
        "drafter/src/refract/InfoElements.cc",
        "drafter/src/refract/InfoElementsUtils.h",
        "drafter/src/refract/ElementFwd.h",
        "drafter/src/refract/ElementIfc.h",
        "drafter/src/refract/Element.h",
        "drafter/src/refract/Element.cc",
        "drafter/src/refract/TypeQueryVisitor.h",
        "drafter/src/refract/TypeQueryVisitor.cc",
        "drafter/src/refract/VisitorUtils.h",
        "drafter/src/refract/VisitorUtils.cc",
        "drafter/src/refract/Visitor.h",

        "drafter/src/refract/ComparableVisitor.h",
        "drafter/src/refract/ComparableVisitor.cc",
        "drafter/src/refract/IsExpandableVisitor.h",
        "drafter/src/refract/IsExpandableVisitor.cc",
        "drafter/src/refract/ExpandVisitor.h",
        "drafter/src/refract/ExpandVisitor.cc",
        "drafter/src/refract/PrintVisitor.h",
        "drafter/src/refract/PrintVisitor.cc",
        "drafter/src/refract/FilterVisitor.h",
        "drafter/src/refract/JsonSchema.h",
        "drafter/src/refract/JsonSchema.cc",
        "drafter/src/refract/JsonValue.h",
        "drafter/src/refract/JsonValue.cc",
        "drafter/src/refract/JsonUtils.h",
        "drafter/src/refract/JsonUtils.cc",
        "drafter/src/refract/ElementUtils.h",
        "drafter/src/refract/ElementUtils.cc",
        "drafter/src/refract/SerializeSo.h",
        "drafter/src/refract/SerializeSo.cc",

        "drafter/src/refract/Registry.h",
        "drafter/src/refract/Registry.cc",

        "drafter/src/refract/Query.h",
        "drafter/src/refract/Query.cc",
        "drafter/src/refract/Iterate.h",

        "drafter/src/refract/dsd/Utils.h",
        "drafter/src/refract/dsd/Array.h",
        "drafter/src/refract/dsd/Bool.h",
        "drafter/src/refract/dsd/Enum.h",
        "drafter/src/refract/dsd/Extend.h",
        "drafter/src/refract/dsd/Holder.h",
        "drafter/src/refract/dsd/Member.h",
        "drafter/src/refract/dsd/Null.h",
        "drafter/src/refract/dsd/Number.h",
        "drafter/src/refract/dsd/Object.h",
        "drafter/src/refract/dsd/Option.h",
        "drafter/src/refract/dsd/Ref.h",
        "drafter/src/refract/dsd/Select.h",
        "drafter/src/refract/dsd/String.h",
        "drafter/src/refract/dsd/Traits.h",

        "drafter/src/refract/dsd/Array.cc",
        "drafter/src/refract/dsd/Bool.cc",
        "drafter/src/refract/dsd/Enum.cc",
        "drafter/src/refract/dsd/Extend.cc",
        "drafter/src/refract/dsd/Holder.cc",
        "drafter/src/refract/dsd/Member.cc",
        "drafter/src/refract/dsd/Null.cc",
        "drafter/src/refract/dsd/Number.cc",
        "drafter/src/refract/dsd/Object.cc",
        "drafter/src/refract/dsd/Option.cc",
        "drafter/src/refract/dsd/Ref.cc",
        "drafter/src/refract/dsd/Select.cc",
        "drafter/src/refract/dsd/String.cc",
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
        'ext/Catch/single_include',
        'ext/dtl/dtl',
        'drafter/src/',
      ],
      'sources': [
        "drafter/test/test-drafter.cc",
        "drafter/test/test-RefractDataStructureTest.cc",
        "drafter/test/test-RefractAPITest.cc",
        "drafter/test/test-RefractParseResultTest.cc",
        "drafter/test/test-RenderTest.cc",
        "drafter/test/test-RefractSourceMapTest.cc",
        "drafter/test/test-SchemaTest.cc",
        "drafter/test/test-CircularReferenceTest.cc",
        "drafter/test/test-ApplyVisitorTest.cc",
        "drafter/test/test-ExtendElementTest.cc",
        "drafter/test/test-ElementFactoryTest.cc",
        "drafter/test/test-OneOfTest.cc",
        "drafter/test/test-SyntaxIssuesTest.cc",
        "drafter/test/test-ElementDataTest.cc",
        "drafter/test/test-Serialize.cc",

        "drafter/test/utils/test-Variant.cc",
        "drafter/test/utils/test-Utf8.cc",
        "drafter/test/utils/so/test-JsonIo.cc",
        "drafter/test/utils/so/test-YamlIo.cc",

        "drafter/test/refract/test-Utils.cc",
        "drafter/test/refract/test-JsonSchema.cc",
        "drafter/test/refract/test-JsonValue.cc",

        "drafter/test/refract/dsd/test-Array.cc",
        "drafter/test/refract/dsd/test-Bool.cc",
        "drafter/test/refract/dsd/test-Enum.cc",
        "drafter/test/refract/dsd/test-Extend.cc",
        "drafter/test/refract/dsd/test-Holder.cc",
        "drafter/test/refract/dsd/test-Member.cc",
        "drafter/test/refract/dsd/test-Null.cc",
        "drafter/test/refract/dsd/test-Number.cc",
        "drafter/test/refract/dsd/test-Object.cc",
        "drafter/test/refract/dsd/test-Option.cc",
        "drafter/test/refract/dsd/test-Ref.cc",
        "drafter/test/refract/dsd/test-Select.cc",
        "drafter/test/refract/dsd/test-String.cc",

        "drafter/test/refract/dsd/test-Element.cc",
        "drafter/test/refract/dsd/test-InfoElements.cc",
        "drafter/test/refract/test-InfoElementsUtils.cc",

        "drafter/test/test-ElementInfoUtils.cc",
        "drafter/test/test-ElementComparator.cc",
        "drafter/test/test-VisitorUtils.cc",

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
        "drafter/src/main.cc",
        "drafter/src/config.cc",
        "drafter/src/config.h",
        "drafter/src/reporting.cc",
        "drafter/src/reporting.h",
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
        "drafter/test/test-CAPI.c"
      ],
      "dependencies": [
        "libdrafter",
      ],
    },
  ],
}
