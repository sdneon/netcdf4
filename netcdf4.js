const netcdf4 = require('./build/Release/netcdf4.node');
module.exports = netcdf4;

/*
Given the key values to all dimensions, find the lowest (L) matching dimensions,
and return that indexed value.

@param rootVar (netcdf4 data) root.variables parent of this object;
                      used to read other dimensions used for indexing into this object's data
@param keys (number or array of numbers) single key value for 1D data,
                      or array of key values for multi-dimensional data.
@param optDesiredDimIndex (int; optional) optional index of dimension whose data is all to be returned
@retval (undefined, value) The single value read.
                      Or if optDesiredDimIndex is specified (for multi-dimensional data only), all the data for this dimension is returned in an array.
                      Else undefined upon error.

E.g.: 2D { 0,0: 10, 0,1: 20, 1,0: 30, 1,1: 40 }
readL([0, 0.5]) -> returns value at 0,0 -> 20.
readL([0.5, 0.5]) -> returns value at 0,0 -> 10.
*/
netcdf4.Variable.prototype.readL = function(rootVar, keys, optDesiredDimIndex)
{
    let i, v;
    if (keys instanceof Array)
    {
        //1. Check that number of dimensions match
        const maxIndices = this.dims;
        if (keys.length < maxIndices.length)
        {
            console.log('ERROR: insufficient keys provided. Got', keys.length, 'instead of', maxIndices.length);
            return;
        }

        const self = this;
        function findLowestIndex(dimIndex)
        {
            const dim = rootVar[self.dimensions[dimIndex].name],
                maxIndex = maxIndices[dimIndex], //this.dimensions[dimIndex].length
                desiredKey = keys[dimIndex];
            for (let j = 0; j < maxIndex; ++j)
            {
                v = dim.read(j);
                if (v === desiredKey)
                    return j;
                if (v > desiredKey)
                {
                    if (j > 0)
                        return j - 1; //return index of last entry which is smaller
                    return 0; //is 1st entry => no lower entry to return, so return index of 1st entry (i.e. 0) instead
                }
            }
            return maxIndex - 1;
        }
        //2. Find "matching" indices for each dimension
        const retAll = (optDesiredDimIndex !== undefined)
            && (optDesiredDimIndex >= 0) && (optDesiredDimIndex < maxIndices.length);
        const indices = [];
        for (i = 0; i < maxIndices.length; ++i)
        {
            if (!retAll)
            {
                indices.push(findLowestIndex(i));
            }
            else
            {
                if (optDesiredDimIndex !== i)
                {
                    indices.push(findLowestIndex(i)); //pos, size
                    indices.push(1);
                }
                else
                {
                    indices.push(0);
                    indices.push(maxIndices[optDesiredDimIndex]);
                }
            }
        }
        //3. Return values
        if (!retAll)
        {
            return this.read(...indices);
        }
        return this.readSlice(...indices);
    }
    //else simple case of 1D data
    const dim = rootVar[this.dimensions[0].name],
        maxIndex = this.dimensions[0].length;
    for (i = 0; i < maxIndex; ++i)
    {
        v = dim.read(i);
        if (v === keys)
        {
            return this.read(i);
        }
        if (v > keys)
        {
            if (i > 0)
                return this.read(i - 1); //return last entry which is smaller
            return this.read(i); //is 1st entry => no lower entry to return, so return 1st entry instead
        }
    }
    return this.read(maxIndex - 1);
}

/*
Similar to readL but returns the value at the higher matching dimension(s)

E.g.: 2D { 0,0: 10, 0,1: 20, 1,0: 30, 1,1: 40 }
readH([0, 0.5]) -> returns value at 0,1 -> 20.
readH([0.5, 0.5]) -> returns value at 1,1 -> 40.
*/
netcdf4.Variable.prototype.readH = function(rootVar, keys, optDesiredDimIndex)
{
    let i, v;
    if (keys instanceof Array)
    {
        //check that number of dimensions match
        const maxIndices = this.dims;
        if (keys.length < maxIndices.length)
        {
            console.log('ERROR: insufficient keys provided. Got', keys.length, 'instead of', maxIndices.length);
            return;
        }

        const self = this;
        function findHigherIndex(dimIndex)
        {
            const dim = rootVar[self.dimensions[dimIndex].name],
                maxIndex = maxIndices[dimIndex], //this.dimensions[dimIndex].length
                desiredKey = keys[dimIndex];
            for (let i = 0; i < maxIndex; ++i)
            {
                v = dim.read(i);
                if (v >= desiredKey)
                {
                    return i;
                }
            }
            return maxIndex - 1;
        }
        const retAll = (optDesiredDimIndex !== undefined)
            && (optDesiredDimIndex >= 0) && (optDesiredDimIndex < maxIndices.length);
        const indices = [];
        for (i = 0; i < maxIndices.length; ++i)
        {
            if (!retAll)
            {
                indices.push(findHigherIndex(i));
            }
            else
            {
                if (optDesiredDimIndex !== i)
                {
                    indices.push(findHigherIndex(i)); //pos, size
                    indices.push(1);
                }
                else
                {
                    indices.push(0);
                    indices.push(maxIndices[optDesiredDimIndex]);
                }
            }
        }
        //3. Return values
        if (!retAll)
        {
            return this.read(...indices);
        }
        return this.readSlice(...indices);
    }
    //else simple case of 1D data
    const dim = rootVar[this.dimensions[0].name],
        maxIndex = this.dimensions[0].length;
    for (i = 0; i < maxIndex; ++i)
    {
        v = dim.read(i);
        if (v >= keys)
        {
            return this.read(i); //return current entry
        }
    }
    return this.read(maxIndex - 1);
}

/*
Similar to readL/H but returns simple 50-50 average ((M)idway) based on returns of readL & readH.
It maybe possible to interpolate for 2D only (in future), but we're mostly working with 4D data,
so we won't try to interpolate that complicated web.

E.g.: 2D { 0,0: 10, 0,1: 20, 1,0: 30, 1,1: 40 }
readM([0, 0.5]) -> returns 20.
readM([0.5, 0.5]) -> returns 25.
*/
netcdf4.Variable.prototype.readM = function(rootVar, keys, optDesiredDimIndex)
{
    const v1 = this.readL(rootVar, keys, optDesiredDimIndex),
        v2 = this.readH(rootVar, keys, optDesiredDimIndex);

    if ('number' === typeof v1)
        return (v1 + v2) / 2;
    //else average the arrays
    return v1.map((x, i) => (x + v2[i]) / 2);
}