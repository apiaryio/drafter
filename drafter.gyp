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

        "src/Serialize.h",
        "src/Serialize.cc",
        "src/SerializeAST.h",
        "src/SerializeAST.cc",
        "src/SerializeSourcemap.h",
        "src/SerializeSourcemap.cc",
        "src/SerializeResult.h",
        "src/SerializeResult.cc",
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
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
      ],
    },

    {
      'target_name': 'test-libdrafter',
      'type': 'executable',
      'include_dirs': [
        'src',
        'test',
        'test/vendor/Catch/include',
        "ext/snowcrash/src",
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/sos/src",
      ],
      'sources': [
        "test/test-main.cc",
        "test/test-SerializeResult.cc",
        "test/test-cdrafter.cc",
      ],
      'dependencies': [
        "libdrafter",
        "libsos",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
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
        "libsos",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
      ],
    },

  ],
}
