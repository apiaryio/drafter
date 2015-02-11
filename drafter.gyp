{
  'targets' : [
    {
      'target_name': 'drafter',
      'type': 'executable',
      'sources': [
        'src/Drafter.cc',
      ],
      'dependencies': [
	'./ext/snowcrash/snowcrash.gyp:libsnowcrash',
	'./ext/snowcrash/snowcrash.gyp:libmarkdownparser',
	'./ext/snowcrash/snowcrash.gyp:libsundown',
	'./ext/snowcrash/snowcrash.gyp:libsos',
      ],
    },
  ]
}
