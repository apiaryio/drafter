var Module = {
    'ready': false,
    'log': function(text) { console.log('drafter.js: ' + text); },
    'logErr': function(text) { console.error('drafter.js: ' + text); },
    'onRuntimeInitialized': function() { this.ready = true; }
};
