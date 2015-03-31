Module['parseAPIBlueprint'] = function(text) {

    if (false === this.ready) {
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

Module['validateAPIBlueprint'] = function(text) {

    if (false === this.ready) {
        return -1;
    }

    buffer = _malloc(text.length+1);
    writeStringToMemory(text, buffer);
    res = _drafter_c_parse(buffer, 0, 0);
    _free(buffer);
    return res ;
};

var parseAPIBlueprint = Module['parseAPIBlueprint'];
var validateAPIBlueprint = Module['validateAPIBlueprint'];
