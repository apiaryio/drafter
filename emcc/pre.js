var Module = {
    'ready': false,
    'print': function(text) { console.log('jsdrafter.js: ' + text); },
    'printErr': function(text) { console.error('jsdrafter.js: ' + text); },
    'onRuntimeInitialized': function() { this.ready = true; }
};
