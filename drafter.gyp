{
  "includes": [
    "ext/snowcrash/common.gypi"
  ],
  
  "targets" : [
    {
      "target_name": "drafter",
      "type": "executable",
      "sources": [
        "src/drafter.cc",
      ],

      "include_dirs": [
        "ext/snowcrash/ext/cmdline",
      ],

      "dependencies": [
	"ext/snowcrash/snowcrash.gyp:libsnowcrash",
	"ext/snowcrash/snowcrash.gyp:libmarkdownparser",
	"ext/snowcrash/snowcrash.gyp:libsundown",
	"ext/snowcrash/snowcrash.gyp:libsos",
      ],
    },
  ],
}
