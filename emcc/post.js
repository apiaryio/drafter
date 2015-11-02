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
Module['parse'] = function(blueprint, options) {
  if (false === this.ready) {
    return {err: -1, result: null};
  }

  var chptr = _malloc(4);
  // at most 4 bytes per UTF-8 code point, +1 for the trailing '\0'
  var buffer = _malloc(4*blueprint.length+1);
  var parseOptions = 0;
  var astType = 1;

  writeStringToMemory(blueprint, buffer);

  if (options) {
    if (options.exportSourcemap) {
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
        throw new Error('Unknown type option ' + options.type);
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
    throw err;
  }

  return (options && options.json === false) ? output : JSON.parse(output);
};

var drafter = Module;
