/*
 * Parse an API Blueprint. By default, this will return the result loaded
 * as an object.
 *
 * Options:
 * - `generateSourcemap`: Set to export sourcemap information.
 * - `json`: Set to `false` to disable parsing of the JSON data. You will
             instead get a JSON string as the result.
 * - `requireBlueprintName`: Set to generate an error if the blueprint is
                             missing a title.
 */
Module['parse'] = function(blueprint, options, callback) {
  if (arguments.length < 1 || arguments.length > 3) {
    throw new TypeError('wrong number of arguments, `parse(string, options, callback)` expected');
  }

  if (typeof blueprint !== 'string') {
    throw new TypeError('wrong 1st argument - string expected, `parse(string, options, [callback])`')
  }

  if (options && typeof options === 'function') {
    callback = options;
    options = {};
  }

  options = options || {};

  if (typeof options !== 'object') {
    throw new TypeError('wrong 2nd argument - object expected, `parse(string, options, [callback])`');
  }

  if (callback && typeof callback !== 'function') {
    throw new TypeError('wrong number of arguments, `parse(string, options)` expected');
  }

  var sync = false;

  if (options.sync) {
    sync = true;
    delete options.sync;
  }

  var allowedOptions = ['generateSourceMap', 'exportSourcemap', 'requireBlueprintName'];

  Object.keys(options).forEach(function (key) {
    if (allowedOptions.indexOf(key) === -1) {
      throw new TypeError('unrecognized option \'' + key + '\', expected: \'requireBlueprintName\', \'generateSourceMap\'');
    }
  });

  if (false === this.ready) {
    var err = new Error('Module not ready!');

    if (sync) {
      throw err;
    }

    if (callback) {
      return callback(err);
    }

    return Promise.reject(err);
  }

  try {

    var chptr = _malloc(4);
    var bufferLen = lengthBytesUTF8(blueprint) + 1;
    var buffer = _malloc(bufferLen);
    var requireBlueprintName = false;
    var sourcemap = false;

    stringToUTF8(blueprint, buffer, bufferLen);

    if (options.generateSourceMap || options.exportSourcemap) {
      sourcemap = true;
    }

    if (options.requireBlueprintName) {
      requireBlueprintName = true;
    }

    var res = _c_parse(buffer, requireBlueprintName, sourcemap, chptr);

    _free(buffer);

    var ptrstr = getValue(chptr, '*');
    var output = Pointer_stringify(ptrstr);

    _free(ptrstr);
    _free(chptr);

  } catch (ex) {
    if (sync) {
      throw ex;
    }

    if (callback) {
      return callback(ex);
    }

    return Promise.reject(ex);
  }

  var error = null;
  var result = null;

  if (res < 0) {
    error = new Error('Parser: Unknown Error');
  } else {
    result = (options.json === false) ? output : JSON.parse(output);
  }

  if (sync) {
    if (error) {
      throw error;
    }

    return result;
  }

  if (callback) {
    return callback(error, result);
  }

  if (error) {
    return Promise.reject(error);
  }

  return Promise.resolve(result);
};

Module['parseSync'] = function(blueprint, options) {
  if (arguments.length < 1 || arguments.length > 2) {
    throw new TypeError('wrong number of arguments, `parseSync(string, options)` expected');
  }

  if (typeof blueprint !== 'string') {
    throw new TypeError('wrong 1st argument - string expected, `parseSync(string, options)`')
  }

  options = options || {};

  if (typeof options !== 'object') {
    throw new TypeError('wrong 2nd argument - object expected, `parseSync(string, options)`');
  }

  options.sync = true;

  return Module.parse(blueprint, options);
};

/*
 * Validate an API Blueprint.
 *
 * Options:
 * - `json`: Set to `false` to disable parsing of the JSON data. You will
             instead get a JSON string as the result.
 * - `requireBlueprintName`: Set to generate an error if the blueprint is
                             missing a title.
 */
Module['validate'] = function(blueprint, options, callback) {
  if (arguments.length < 1 || arguments.length > 3) {
    throw new TypeError('wrong number of arguments, `validate(string, options, callback)` expected');
  }

  if (typeof blueprint !== 'string') {
    throw new TypeError('wrong 1st argument - string expected, `validate(string, options, [callback])`')
  }

  if (options && typeof options === 'function') {
    callback = options;
    options = {};
  }

  options = options || {};

  if (typeof options !== 'object') {
    throw new TypeError('wrong 2nd argument - object expected, `validate(string, options, [callback])`');
  }

  if (callback && typeof callback !== 'function') {
    throw new TypeError('wrong number of arguments, `validate(string, options)` expected');
  }

  var sync = false;

  if (options.sync) {
    sync = true;
    delete options.sync;
  }

  var allowedOptions = ['requireBlueprintName'];

  Object.keys(options).forEach(function (key) {
    if (allowedOptions.indexOf(key) === -1) {
      throw new TypeError('unrecognized option \'' + key + '\', expected: \'requireBlueprintName\'');
    }
  });

  if (false === this.ready) {
    var err = new Error('Module not ready!');

    if (sync) {
      throw err;
    }

    if (callback) {
      return callback(err);
    }

    return Promise.reject(err);
  }

  try {

    var chptr = _malloc(4);
    var bufferLen = lengthBytesUTF8(blueprint) + 1;
    var buffer = _malloc(bufferLen);
    var requireBlueprintName = false;
    var output = null;

    if (options.requireBlueprintName) {
      requireBlueprintName = true;
    }

    stringToUTF8(blueprint, buffer, bufferLen);

    var res = _c_validate(buffer, requireBlueprintName, chptr);
    _free(buffer);

    if (res) {
      var ptrstr = getValue(chptr, '*');
      output = (options.json === false) ? Pointer_stringify(ptrstr) : JSON.parse(Pointer_stringify(ptrstr));
      _free(ptrstr);
    }
    _free(chptr);

  } catch (ex) {
    if (sync) {
      throw ex;
    }

    if (callback) {
      return callback(ex);
    }

    return Promise.reject(ex);
  }

  var error = null;
  var result = null;

  if (res < 0) {
    error = new Error('Parser: Unknown Error');
  } else {
    result = output;
  }

  if (sync) {
    if (error) {
      throw error;
    }

    return result;
  }

  if (callback) {
    return callback(error, result);
  }

  if (error) {
    return Promise.reject(error);
  }

  return Promise.resolve(result);
};

Module['validateSync'] = function(blueprint, options) {
  if (arguments.length < 1 || arguments.length > 2) {
    throw new TypeError('wrong number of arguments, `validateSync(string, options)` expected');
  }

  if (typeof blueprint !== 'string') {
    throw new TypeError('wrong 1st argument - string expected, `validateSync(string, options)`')
  }

  options = options || {};

  if (typeof options !== 'object') {
    throw new TypeError('wrong 2nd argument - object expected, `validateSync(string, options)`');
  }

  options.sync = true;

  return Module.validate(blueprint, options);
};
