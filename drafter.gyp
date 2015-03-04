{
  "includes": [
    "ext/snowcrash/common.gypi"
  ],
  
  "targets" : [
    {
      "target_name": "libdrafter",
      'type': '<(libdrafter_type)',

      "sources": [
        "src/config.cc",
        "src/config.h",
        "src/reporting.cc",
        "src/reporting.h",
        "src/stream.h",
        "src/Version.h",

        "src/Serialize.h",
        "src/Serialize.cc",
        "src/SerializeAST.h",
        "src/SerializeAST.cc",
        "src/SerializeSourcemap.h",
        "src/SerializeSourcemap.cc",
        "src/SerializeSourceAnnotations.h",
        "src/SerializeSourceAnnotations.cc",
      ],

      # FIXME: replace by direct dependecies
      "include_dirs": [
        "ext/snowcrash/ext/cmdline",
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/snowcrash/ext/sos/src",
        "ext/snowcrash/src",
      ],

      "dependencies": [
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
        "ext/snowcrash/snowcrash.gyp:libsos",
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
        "ext/snowcrash/ext/sos/src",
      ],
      'sources': [
        "test/test-main.cc",
        "test/test-SerializeSourceAnnotations.cc",
      ],
      'dependencies': [
        "libdrafter",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
        "ext/snowcrash/snowcrash.gyp:libsos",
      ]
    },

    {
      "target_name": "drafter",
      "type": "executable",
      "sources": [
        "src/drafter.cc",
      ],

      # FIXME: replace by direct dependecies
      "include_dirs": [
        "ext/snowcrash/ext/cmdline",
        "ext/snowcrash/ext/markdown-parser/src",
        "ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "ext/snowcrash/ext/sos/src",
        "ext/snowcrash/src",
      ],

      "dependencies": [
        "libdrafter",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
        "ext/snowcrash/snowcrash.gyp:libmarkdownparser",
        "ext/snowcrash/snowcrash.gyp:libsundown",
        "ext/snowcrash/snowcrash.gyp:libsos",
      ],
    },

  ],
}
