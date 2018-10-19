var Module;

if (!Module) {
  Module = {};
}

Module['ready'] = false;
Module['log'] = function(text) { console.log('drafter.js: ' + text); };
Module['logErr'] = function(text) { console.error('drafter.js: ' + text); };
Module['onRuntimeInitialized'] = function() { this.ready = true; };
