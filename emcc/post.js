/*
 * Parse an API Blueprint. By default, this will return the result loaded
 * as an object.
 *
 * Options:
 * - `exportSourcemap`: Set to export sourcemap information.
 * - `json`: Set to `false` to disable parsing of the JSON data. You will
             instead get a JSON string as the result.
 * - `requireBlueprintName`: Set to generate an error if the blueprint is
                             missing a title.
 * - `type`: Either `refract` (default) or `ast`.
 */
Module['parse'] = function(blueprint, options, callback) {
  if (false === this.ready) {
    var err = new Error('Module not ready!');
    if (callback) {
      return callback(err, null);
    }
    return err;
  }

  var chptr = _malloc(4);
  // at most 4 bytes per UTF-8 code point, +1 for the trailing '\0'
  var buffer = _malloc(4*blueprint.length+1);
  var parseOptions = 0;
  var astType = 1;

  writeStringToMemory(blueprint, buffer);

  if (options) {
    if (options.exportSourcemap || options.generateSourceMap) {
      parseOptions |= (1 << 2);
    }

    if (options.requireBlueprintName) {
      parseOptions |= (1 << 1);
    }

    switch (options.type) {
      case 'ast':
        astType = 0;
        break;
      case 'refract':
        astType = 1;
        break;
      case undefined:
        break;
      default:
        var err = new Error('Unknown type option ' + options.type);
        if (callback) {
          return callback(err, null);
        }
        throw err;
    }
  }

  var res = _drafter_c_parse(buffer, parseOptions, astType, chptr);

  _free(buffer);

  var ptrstr = getValue(chptr, '*');
  var output = Pointer_stringify(ptrstr);

  _free(ptrstr);
  _free(chptr);

  if (res) {
    var err = new Error('Error parsing blueprint!');
    err.result = (options && options.json === false) ? output : JSON.parse(output);
    if (callback) {
      return callback(err, err.result);
    }
    throw err;
  }
  if (callback) {
    return callback(null, (options && options.json === false) ? output : JSON.parse(output));
  }
  return (options && options.json === false) ? output : JSON.parse(output);
};

Module['parseSync'] = function(blueprint, options) {
  return Module.parse(blueprint, options);
};

var drafter = Module;
