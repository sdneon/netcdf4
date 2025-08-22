# netcdf4-js for Node.JS 24.x (mod)

Convenient package to use netcdf4 addon in Node.JS 24.x in Windows OS.

### New API
New APIs for ease of use.
* **`Variable.dims`** - Return single dimension directly as number, or multiple dimensions as array of numbers.
  ```js
    const netcdf4 = require("netcdf4");
    const data = new netcdf4.File("test/testrh.nc", "r");
    //Instead of:
    //const max = data.variables.var1.dimensions[0].length; //returns 10000
    //Simply:
    const max = data.variables.var1.dims; //returns 10000
    
    //For 2D (not in testrh.nc; sample code only):
    const max2d = data.variables.var2d.dims; //returns [10, 20]
    /*
    where:
        data.variables.var2d.dimensions.length = 2
        data.variables.var2d.dimensions[0].length = 10
        data.variables.var2d.dimensions[1].length = 20
    */
  ```

* **`Variable.readL`**(rootVar, keys, optDesiredDimIndex) - Get associated value(s) for given keys (dimension values).
  * For 1D data, `keys` should be simply the value in that single dimension (exact or next lower value is matched), and its corresponding data value is returned.
  * For multi-dimensional data, `keys` should be an array of all respective values in all dimensions. The single corresponding data value is returned.
    * Optional `optDesiredDimIndex`: If specified, indicates that you wish to rerieve the entire set of data values for all values of this index dimension. (Corresponding value in `keys` will be ignored). Array of this slice of data is returned.
  ```js
    //1D:
    const temperatureAtNoon = data.read(noon);
    
    //4D single value:
    const airDensityAtPointAtNoon = data.read([noon, latitude, longitube, altitude]);
    
    //4D slice of values:
    const airDensityAtPointFor24hrs = data.read([ignored, latitude, longitube, altitude], 0);
  ```
  * Similarly for **`Variable.readH`** and **`Variable.readM`**.
    * `readH` gets 'rounds up' to the next higher value in the dimensions instead.
    * `readM` gets the average between readL & readH results.
      * Simple interpolation could have been done for 1D data, but the focus here is on multi-dimensional data, so  asimpel averaging is used instead of some complex interpolation.
  
## Developer Notes
Bindings (*.cpp) have been updated to Node.JS 24.x's API.

* Built using:
  * Visual Studio 2022
  * netCDF v4.9.3 (2023-03-14)
* Dependencies are in `deps` folder.
  * `netCDF` is an installation of netCDF (v4.9.3; 2023-03-14) from [unidata](https://downloads.unidata.ucar.edu/netcdf/).
  * `nodejs` contains Node.JS (v24.4.1) header/library files and was pulled by node-gyp.
  * `node-gyp`
* Outputs:
  * build/Release/netcdf4.node
  * build/Debug/netcdf4.node

### Wishlist
Simpler APIs.

# netcdf4-js (original readme)

![Build status](https://github.com/parro-it/netcdf4/workflows/Node.js%20CI/badge.svg?branch=master)
[![NPM Version](https://img.shields.io/npm/v/netcdf4.svg)](https://npmjs.org/package/netcdf4)


NodeJS addon for reading and writing the files in the
[Network Common Data Form (NetCDF)](https://www.unidata.ucar.edu/software/netcdf/)
version <= 4,
built upon the C-library for netcdf.

## Installation

`netcdf4-js` is built with `nodejs` >= 4.x

Install using `npm`:

```
$ npm install netcdf4
```

Prerequisites:

You will need `libnetcdf` >= 4.x installed.

### On Linux/Unix/OSX

* Install NetCDF4 using your package manager, e.g., on Ubuntu/Debian:
```
$ sudo apt-get install libnetcdf-dev
```
or download it [here](https://www.unidata.ucar.edu/downloads/netcdf/index.jsp)
* Make sure your system fulfills all the prerequisites of [node-gyp](https://github.com/nodejs/node-gyp#on-unix)

### On Windows:

* Install NetCDF4 from [here](https://www.unidata.ucar.edu/downloads/netcdf/index.jsp)
* Make sure to select at least "dependencies", "headers" and "libraries" to install in the NetCDF installation wizard
* Install the build tools as described [here](https://github.com/nodejs/node-gyp#on-windows)
* Set the environment variable `NETCDF_DIR` to your NetCDF installation, e.g.,
``` bash
C:\> set NETCDF_DIR=C:\Program Files\netCDF 4.6.1
```


## Usage

Open files with
```
var netcdf4 = require("netcdf4");

var file = new netcdf4.File("test/testrh.nc", "r");
```
File modes are `"r"` for "reading", `"w"` for "writing", `"c"` for
"creation", and `"c!"` for "overwriting".

Then you can read variables using `read` or `readSlice`. The following example reads values at positions 5 to 15:
```
console.log(file.root.variables['var1'].readSlice(5, 10));
```

### Classes

Properties marked *(r/w)* can be read and will be written to the file
when set.

#### File

Properties:
* `root` : Main `Group`-object in file

Methods:
* `close()` : Close file
* `sync()` : Sync (or "flush") file to disk

#### Group

Properties:
* `id` : ID used by C-library
* `name` : Name
* `fullname` : Full name (path in file)
* `variables` : Associative array of variables in group
* `dimensions` : Associative array of dimensions in group
* `unlimited` : Associative array of unlimited Dimensions in group
* `attribute` : Associative array of attributes of group
* `subgroups` : Associative array of subgroups of group

Methods:
* `addVariable(name, type, dimensions)` : Add a new variable in
  group. `type` is one of `"byte", "char", "short", "int", "ubyte", "ushort",
  "uint", "float", "double"`. `dimensions` is an array of ids of dimensions
  for the new variable. Returns new variable.
* `addDimension(name, length)` : Add new dimension of length `length`
  (can be `"unlimited"` for unlimited dimension). Returns new dimension.
* `addSubgroup(name)` : Add subgroup. Returns new subgroup.
* `addAttribute(name, type, value)` : Add and set new attribute. Returns new attribute.

### Dimension

Properties:
* `id` : ID used by C-library
* `name` : Name (r/w)
* `length` : Length or currently used length for unlimited dimensions

### Attribute

Properties:
* `id` : ID used by C-library
* `name` : Name (r/w)
* `value` : Value (r/w)

Methods:
* `delete()` : Delete attribute

### Variable

Properties:
* `id` : ID used by C-library
* `name` : Name (r/w)
* `type` : Type of variable
* `attributes` : Associative array of attributes
* `dimensions` : Array of dimensions used by variable
* `endianness` : Endianness: `"little"`, `"big"`, or `"native"` (r/w)
* `checksummode` : Checksum mode: `"none"`, or `"fletcher32"` (r/w)
* `chunkmode` : Chunk mode: `"contiguous"`, or `"chunked"` (r/w)
* `chunksizes` : Array of chunk sizes (one size per dimension) (r/w)
* `fillmode` : Boolean switch for fill mode (r/w)
* `fillvalue` : Fill value (r/w)
* `compressionshuffle` : Boolean switch for shuffle (r/w)
* `compressiondeflate` : Boolean switch for compression (r/w)
* `compressionlevel` : Compression level (1-9) (r/w)

Methods:
* `read(pos....)` : Reads and returns a single value at positions
  given as for `write`.
* `readSlice(pos, size....)` : Reads and returns an array of values (cf.
  ["Specify a Hyperslab"](https://www.unidata.ucar.edu/software/netcdf/docs/programming_notes.html#specify_hyperslab))
  at positions and sizes given for each dimension, `readSlice(pos1,
  size1, pos2, size2, ...)` e.g. `readSlice(2, 3, 4, 2)` gives an
  array of the values at position 2 for 3 steps along the first
  dimension and position 4 for 2 steps along the second one.
* `readStridedSlice(pos, size, stride....)` : Similar to `readSlice()`, but it
  adds a *stride* (interval between indices) parameter to each dimension. If stride is 4,
  the function will take 1 value, discard 3, take 1 again, etc.
  So for instance `readStridedSlice(2, 3, 2, 4, 2, 1)` gives an
  array of the values at position 2 for 3 steps with stride 2 (i.e.
  every other value) along the first dimension and position 4 for 2 steps
  with stride 1 (i.e. with no dropping) along the second dimension.
* `write(pos..., value)` : Write `value` at positions given,
  e.g. `write(2, 3, "a")` writes `"a"` at position 2 along the first
  dimension and position 3 along the second one.
* `writeSlice(pos, size..., valuearray)` : Write values in `valuearray`
  (must be a typed array) at positions and sizes given for each
  dimension, e.g. `writeSlice(2, 3, 4, 2, new
  Int32Array([0, 1, 2, 3, 4, 5]))` writes the array at position 2 for
  3 steps along the first dimension and position 4 for 2 step along
  the second one (cf.
  ["Specify a Hyperslab"](https://www.unidata.ucar.edu/software/netcdf/docs/programming_notes.html#specify_hyperslab)).
* `writeStridedSlice(pos, size, stride..., valuearray)` : Similar to
  `writeSlice()`, but it adds a *stride* parameter to each dimension.
  So for instance `writeStridedSlice(2, 3, 2, 4, 2, 1), new
  Int32Array([0, 1, 2, 3, 4, 5])` writes the array
  at position 2 for 3 steps with stride 2 (i.e.
  every other value) along the first dimension and position 4 for 2 steps
  with stride 1 (i.e. with no dropping) along the second dimension.
* `addAttribute(name, type, value)` : Adds and sets new attribute. Returns
  new attribute.
