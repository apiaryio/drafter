var jsDrafterReady = false;

var Module = {
    'print': function(text) { console.log('jsdrafter.js: ' + text); },
    'printErr': function(text) { console.log('jsdrafter.js stderr: ' + text); },
    'onRuntimeInitialized': function() { jsDrafterReady = true; }
};

/* JSDRAFTER.RAW.JS */

var parseAPIBlueprint = function(text) {

    if (false == jsDrafterReady) {
        return {err: -1, result: ""};
    }

    chptr = _malloc(4);
    buffer = _malloc(text.length+1);
    writeStringToMemory(text, buffer);
    res = _drafter_c_parse(buffer, 0, chptr);
    _free(buffer);
    ptrstr = getValue(chptr, '*');
    parse = Pointer_stringify(ptrstr);
    _free(ptrstr);
    _free(chptr);
    return {err: res, result: parse};
};

var validateAPIBlueprint = function(text) {

    if (false == jsDrafterReady) {
        return -1;
    }

    buffer = _malloc(text.length+1);
    writeStringToMemory(text, buffer);
    res = _drafter_c_parse(buffer, 0, 0);
    _free(buffer);
    return res ;
};
