#include "Variable.h"
#include "Attribute.h"
#include "Dimension.h"
#include "netcdf4js.h"

namespace netcdf4js {

const unsigned char Variable::type_sizes[] = {
    0,  // NC_NAT // unknown type
    1,  // NC_BYTE
    1,  // NC_CHAR
    2,  // NC_SHORT
    4,  // NC_INT / NC_LONG
    4,  // NC_FLOAT
    8,  // NC_DOUBLE
    1,  // NC_UBYTE
    2,  // NC_USHORT
    4   // NC_UINT
};

const char* Variable::type_names[] = {
    "unknown",  // NC_NAT // unknown type
    "byte",     // NC_BYTE
    "char",     // NC_CHAR
    "short",    // NC_SHORT
    "int",      // NC_INT / NC_LONG
    "float",    // NC_FLOAT
    "double",   // NC_DOUBLE
    "ubyte",    // NC_UBYTE
    "ushort",   // NC_USHORT
    "uint"      // NC_UINT
};

v8::Persistent<v8::Function> Variable::constructor;

Variable::Variable(const int& id_, const int& parent_id_) : id(id_), parent_id(parent_id_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
    int retval = nc_inq_var(parent_id, id, NULL, &type, &ndims, NULL, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();

    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Variable", v8::NewStringType::kNormal).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "read", Variable::Read);
    NODE_SET_PROTOTYPE_METHOD(tpl, "readSlice", Variable::ReadSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "readStridedSlice", Variable::ReadStridedSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "write", Variable::Write);
    NODE_SET_PROTOTYPE_METHOD(tpl, "writeSlice", Variable::WriteSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "writeStridedSlice", Variable::WriteStridedSlice);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addAttribute", Variable::AddAttribute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Variable::Inspect);

    FN_GETTER(Variable, Id)
    FN_GETTER(Variable, Type)
    FN_GETTER(Variable, Dimensions)
    FN_GETTER(Variable, Dims)
    FN_GETTER(Variable, Attributes)
    FN_GETTER_AND_SETTER(Variable, Name)
    FN_GETTER_AND_SETTER(Variable, Endianness)
    FN_GETTER_AND_SETTER(Variable, ChecksumMode)
    FN_GETTER_AND_SETTER(Variable, ChunkMode)
    FN_GETTER_AND_SETTER(Variable, ChunkSizes)
    FN_GETTER_AND_SETTER(Variable, FillMode)
    FN_GETTER_AND_SETTER(Variable, FillValue)
    FN_GETTER_AND_SETTER(Variable, CompressionShuffle)
    FN_GETTER_AND_SETTER(Variable, CompressionDeflate)
    FN_GETTER_AND_SETTER(Variable, CompressionLevel)

    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "id", v8::NewStringType::kNormal).ToLocalChecked(), getter_Id);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "type", v8::NewStringType::kNormal).ToLocalChecked(), getter_Type);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "dimensions", v8::NewStringType::kNormal).ToLocalChecked(), getter_Dimensions);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "dims", v8::NewStringType::kNormal).ToLocalChecked(), getter_Dims);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "attributes", v8::NewStringType::kNormal).ToLocalChecked(), getter_Attributes);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), getter_Name, setter_Name);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "endianness", v8::NewStringType::kNormal).ToLocalChecked(), getter_Endianness, setter_Endianness);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "checksummode", v8::NewStringType::kNormal).ToLocalChecked(), getter_ChecksumMode, setter_ChecksumMode);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "chunkmode", v8::NewStringType::kNormal).ToLocalChecked(), getter_ChunkMode, setter_ChunkMode);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "chunksizes", v8::NewStringType::kNormal).ToLocalChecked(), getter_ChunkSizes, setter_ChunkSizes);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "fillmode", v8::NewStringType::kNormal).ToLocalChecked(), getter_FillMode, setter_FillMode);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "fillvalue", v8::NewStringType::kNormal).ToLocalChecked(), getter_FillValue, setter_FillValue);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "compressionshuffle", v8::NewStringType::kNormal).ToLocalChecked(), getter_CompressionShuffle,
        setter_CompressionShuffle);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "compressiondeflate", v8::NewStringType::kNormal).ToLocalChecked(), getter_CompressionDeflate,
        setter_CompressionDeflate);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "compressionlevel", v8::NewStringType::kNormal).ToLocalChecked(), getter_CompressionLevel, setter_CompressionLevel);
    constructor.Reset(isolate, tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

bool Variable::get_name(char* name) const {
    int retval = nc_inq_varname(parent_id, id, name);
    if (retval != NC_NOERR) {
        throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

void Variable::Write(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = 1;
    }
    int retval;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_SHORT: {
            int16_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_INT: {
            int32_t v = args[obj->ndims]->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(args[obj->ndims]->NumberValue(isolate->GetCurrentContext()).ToChecked());
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_DOUBLE: {
            double v = args[obj->ndims]->NumberValue(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        case NC_UINT: {
            uint32_t v = args[obj->ndims]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_put_vara(obj->parent_id, obj->id, pos, size, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
}

void Variable::WriteSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != 2 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (!args[2 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[2 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[2 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[2 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal).ToLocalChecked()));
        delete[] pos;
        delete[] size;
        return;
    }

    bool correct_type;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            correct_type = val->IsInt8Array();
            break;
        case NC_SHORT:
            correct_type = val->IsInt16Array();
            break;
        case NC_INT:
            correct_type = val->IsInt32Array();
            break;
        case NC_FLOAT:
            correct_type = val->IsFloat32Array();
            break;
        case NC_DOUBLE:
            correct_type = val->IsFloat64Array();
            break;
        case NC_UBYTE:
            correct_type = val->IsUint8Array();
            break;
        case NC_USHORT:
            correct_type = val->IsUint16Array();
            break;
        case NC_UINT:
            correct_type = val->IsUint32Array();
            break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal).ToLocalChecked()));
        delete[] pos;
        delete[] size;
        return;
    }
    int retval = nc_put_vara(obj->parent_id, obj->id, pos, size, val->Buffer()->Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
}

void Variable::WriteStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != 3 * obj->ndims + 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (!args[3 * obj->ndims]->IsTypedArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a typed array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    ptrdiff_t* stride = new ptrdiff_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[3 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[3 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
        stride[i] = static_cast<ptrdiff_t>(args[3 * i + 2]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
    }
    v8::Local<v8::TypedArray> val = v8::Local<v8::TypedArray>::Cast(args[3 * obj->ndims]);
    if (val->Length() != total_size) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong size of array", v8::NewStringType::kNormal).ToLocalChecked()));
        delete[] pos;
        delete[] size;
        delete[] stride;
        return;
    }

    bool correct_type;
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            correct_type = val->IsInt8Array();
            break;
        case NC_SHORT:
            correct_type = val->IsInt16Array();
            break;
        case NC_INT:
            correct_type = val->IsInt32Array();
            break;
        case NC_FLOAT:
            correct_type = val->IsFloat32Array();
            break;
        case NC_DOUBLE:
            correct_type = val->IsFloat64Array();
            break;
        case NC_UBYTE:
            correct_type = val->IsUint8Array();
            break;
        case NC_USHORT:
            correct_type = val->IsUint16Array();
            break;
        case NC_UINT:
            correct_type = val->IsUint32Array();
            break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            delete[] stride;
            return;
    }
    if (!correct_type) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array type", v8::NewStringType::kNormal).ToLocalChecked()));
        delete[] pos;
        delete[] size;
        delete[] stride;
        return;
    }
    int retval = nc_put_vars(obj->parent_id, obj->id, pos, size, stride, val->Buffer()->Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] pos;
    delete[] size;
    delete[] stride;
}

void Variable::Read(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = 1;
    }
    v8::Local<v8::Value> result;
    int retval;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::String::NewFromUtf8(isolate, v, v8::NewStringType::kNormal).ToLocalChecked();
        } break;
        case NC_SHORT: {
            int16_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            retval = nc_get_vara(obj->parent_id, obj->id, pos, size, &v);
            result = v8::Integer::New(isolate, v);
        } break;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    } else {
        args.GetReturnValue().Set(result);
    }
    delete[] pos;
    delete[] size;
}

void Variable::ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != 2 * obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[2 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[2 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    int retval = nc_get_vara(obj->parent_id, obj->id, pos, size, buffer->Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] pos;
        delete[] size;
        return;
    }
    v8::Local<v8::Object> result;

    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            result = v8::Int8Array::New(buffer, 0, total_size);
            break;
        case NC_SHORT:
            result = v8::Int16Array::New(buffer, 0, total_size);
            break;
        case NC_INT:
            result = v8::Int32Array::New(buffer, 0, total_size);
            break;
        case NC_FLOAT:
            result = v8::Float32Array::New(buffer, 0, total_size);
            break;
        case NC_DOUBLE:
            result = v8::Float64Array::New(buffer, 0, total_size);
            break;
        case NC_UBYTE:
            result = v8::Uint8Array::New(buffer, 0, total_size);
            break;
        case NC_USHORT:
            result = v8::Uint16Array::New(buffer, 0, total_size);
            break;
        case NC_UINT:
            result = v8::Uint32Array::New(buffer, 0, total_size);
            break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            delete[] pos;
            delete[] size;
            return;
    }
    args.GetReturnValue().Set(result);
    delete[] pos;
    delete[] size;
}

void Variable::ReadStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() != 3 * obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    size_t* pos = new size_t[obj->ndims];
    size_t* size = new size_t[obj->ndims];
    ptrdiff_t* stride = new ptrdiff_t[obj->ndims];
    size_t total_size = 1;
    for (int i = 0; i < obj->ndims; i++) {
        pos[i] = static_cast<size_t>(args[3 * i]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size_t s = static_cast<size_t>(args[3 * i + 1]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
        size[i] = s;
        total_size *= s;
        stride[i] = static_cast<ptrdiff_t>(args[3 * i + 2]->IntegerValue(isolate->GetCurrentContext()).ToChecked());
    }
    v8::Local<v8::ArrayBuffer> buffer = v8::ArrayBuffer::New(isolate, total_size * type_sizes[obj->type]);
    int retval = nc_get_vars(obj->parent_id, obj->id, pos, size, stride, buffer->Data());
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] pos;
        delete[] size;
        delete[] stride;
        return;
    }
    v8::Local<v8::Object> result;

    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR:
            result = v8::Int8Array::New(buffer, 0, total_size);
            break;
        case NC_SHORT:
            result = v8::Int16Array::New(buffer, 0, total_size);
            break;
        case NC_INT:
            result = v8::Int32Array::New(buffer, 0, total_size);
            break;
        case NC_FLOAT:
            result = v8::Float32Array::New(buffer, 0, total_size);
            break;
        case NC_DOUBLE:
            result = v8::Float64Array::New(buffer, 0, total_size);
            break;
        case NC_UBYTE:
            result = v8::Uint8Array::New(buffer, 0, total_size);
            break;
        case NC_USHORT:
            result = v8::Uint16Array::New(buffer, 0, total_size);
            break;
        case NC_UINT:
            result = v8::Uint32Array::New(buffer, 0, total_size);
            break;
    }
    args.GetReturnValue().Set(result);
    delete[] pos;
    delete[] size;
    delete[] stride;
}

void Variable::AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(args.This());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    Attribute* res = new Attribute(*v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                       isolate,
#endif
                                       args[0]),
                                   obj->id, obj->parent_id, type);
    res->set_value(args[2]);
    args.GetReturnValue().Set(res->handle());
}

void Variable::GetId(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
}

void Variable::GetDimensions(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int* dim_ids = new int[obj->ndims];
    int retval = nc_inq_vardimid(obj->parent_id, obj->id, dim_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dim_ids;
        return;
    }
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        Dimension* d = new Dimension(dim_ids[i], obj->parent_id);
        result->Set(isolate->GetCurrentContext(), i, d->handle());
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
}

/*
* Retrieve the actual lengths of all dimensions.
* @retval (int or array of int) For single dimension, returns the length itself;
*                               For multiple dimensions, returns lengths in an array.
*/
void Variable::GetDims(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int* dim_ids = new int[obj->ndims];
    int retval = nc_inq_vardimid(obj->parent_id, obj->id, dim_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dim_ids;
        return;
    }
    if (obj->ndims == 1)
    {
        info.GetReturnValue().Set(v8::Integer::New(isolate, obj->ndims));
        return;
    }
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    size_t len;
    for (int i = 0; i < obj->ndims; i++) {
        Dimension* d = new Dimension(dim_ids[i], obj->parent_id);
        result->Set(ctx, i, v8::Integer::New(isolate, d->getLen()));
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
}

void Variable::GetAttributes(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int natts;
    int retval = nc_inq_varnatts(obj->parent_id, obj->id, &natts);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < natts; i++) {
        retval = nc_inq_attname(obj->parent_id, obj->id, i, name);
        if (retval != NC_NOERR) {
            throw_netcdf_error(isolate, retval);
            return;
        }
        Attribute* a = new Attribute(name, obj->id, obj->parent_id);
        result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), a->handle());
    }
    info.GetReturnValue().Set(result);
}

void Variable::GetType(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    const char* res;
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        res = "Variable type not supported yet";
    } else {
        res = type_names[obj->type];
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
}

void Variable::GetName(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked());
    }
}

void Variable::SetName(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int retval = nc_rename_var(obj->parent_id, obj->id, *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetEndianness(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_endian(obj->parent_id, obj->id, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    const char* res;
    switch (v) {
        case NC_ENDIAN_LITTLE:
            res = "litte";
            break;
        case NC_ENDIAN_BIG:
            res = "big";
            break;
        case NC_ENDIAN_NATIVE:
            res = "native";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
}

void Variable::SetEndianness(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "little") {
        v = NC_ENDIAN_LITTLE;
    } else if (arg == "big") {
        v = NC_ENDIAN_BIG;
    } else if (arg == "native") {
        v = NC_ENDIAN_NATIVE;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int retval = nc_def_var_endian(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetChecksumMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_fletcher32(obj->parent_id, obj->id, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    const char* res;
    switch (v) {
        case NC_NOCHECKSUM:
            res = "none";
            break;
        case NC_FLETCHER32:
            res = "fletcher32";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
}

void Variable::SetChecksumMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "none") {
        v = NC_NOCHECKSUM;
    } else if (arg == "fletcher32") {
        v = NC_FLETCHER32;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int retval = nc_def_var_fletcher32(obj->parent_id, obj->id, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Variable::GetChunkMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    const char* res;
    switch (v) {
        case NC_CONTIGUOUS:
            res = "contiguous";
            break;
        case NC_CHUNKED:
            res = "chunked";
            break;
        default:
            res = "unknown";
            break;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, res, v8::NewStringType::kNormal).ToLocalChecked());
}

void Variable::SetChunkMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    std::string arg = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int v;
    if (arg == "contiguous") {
        v = NC_CONTIGUOUS;
    } else if (arg == "chunked") {
        v = NC_CHUNKED;
    } else {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown value", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int len;
    int retval = nc_inq_varndims(obj->parent_id, obj->id, &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    size_t* sizes = new size_t[len];
    retval = nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] sizes;
        return;
    }
    retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] sizes;
}

void Variable::GetChunkSizes(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    size_t* sizes = new size_t[obj->ndims];
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, NULL, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] sizes;
        return;
    }
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    for (int i = 0; i < obj->ndims; i++) {
        result->Set(isolate->GetCurrentContext(), i, v8::Integer::New(isolate, i));
    }
    info.GetReturnValue().Set(result);
    delete[] sizes;
}

void Variable::SetChunkSizes(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    if (!val->IsArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting an array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    v8::Local<v8::Object> array = val->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    if (array->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->Int32Value(isolate->GetCurrentContext()).ToChecked() != obj->ndims) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong array size", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int v;
    int retval = nc_inq_var_chunking(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    size_t* sizes = new size_t[obj->ndims];
    for (int i = 0; i < obj->ndims; i++) {
        sizes[i] = array->Get(isolate->GetCurrentContext(), i).ToLocalChecked()->Uint32Value(isolate->GetCurrentContext()).ToChecked();
    }
    retval = nc_def_var_chunking(obj->parent_id, obj->id, v, sizes);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] sizes;
}

void Variable::GetFillMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetFillMode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
    if (obj->type < NC_BYTE || obj->type > NC_UINT) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    uint8_t* value = new uint8_t[type_sizes[obj->type]];
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, value);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] value;
        return;
    }
    retval = nc_def_var_fill(obj->parent_id, obj->id, v, value);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
    delete[] value;
}

void Variable::GetFillValue(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> result;
    int retval;
    switch (obj->type) {
        case NC_BYTE: {
            int8_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_CHAR: {
            char v[2];
            v[1] = 0;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::String::NewFromUtf8(isolate, v, v8::NewStringType::kNormal).ToLocalChecked();
        } break;
        case NC_SHORT: {
            int16_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_INT: {
            int32_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_FLOAT: {
            float v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_DOUBLE: {
            double v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Number::New(isolate, v);
        } break;
        case NC_UBYTE: {
            uint8_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_USHORT: {
            uint16_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        case NC_UINT: {
            uint32_t v;
            retval = nc_inq_var_fill(obj->parent_id, obj->id, NULL, &v);
            result = v8::Integer::New(isolate, v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(result);
}

void Variable::SetFillValue(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int mode;
    int retval = nc_inq_var_fill(obj->parent_id, obj->id, &mode, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    v8::Local<v8::Value> val = info[0];
    switch (obj->type) {
        case NC_BYTE:
        case NC_CHAR: {
            int8_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_SHORT: {
            int16_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_INT: {
            int32_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_FLOAT: {
            float v = static_cast<float>(val->NumberValue(isolate->GetCurrentContext()).ToChecked());
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_DOUBLE: {
            double v = val->NumberValue(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UBYTE: {
            uint8_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_USHORT: {
            uint16_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        case NC_UINT: {
            uint32_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
            retval = nc_def_var_fill(obj->parent_id, obj->id, mode, &v);
        } break;
        default:
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
            return;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionShuffle(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v, NULL, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetCompressionShuffle(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
    int v1, v2;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v1, &v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v, v1, v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionDeflate(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, &v, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Boolean::New(isolate, v == 1));
}

void Variable::SetCompressionDeflate(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    if (!val->IsBoolean()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a boolean", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
#if NODE_MAJOR_VERSION > 11
    int v = val->BooleanValue(isolate) ? 1 : 0;
#else
    int v = val->BooleanValue() ? 1 : 0;
#endif
    int v1, v2;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v1, NULL, &v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v1, v, v2);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::GetCompressionLevel(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    int v;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, NULL, NULL, &v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Integer::New(isolate, v));
}

void Variable::SetCompressionLevel(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Variable* obj = node::ObjectWrap::Unwrap<Variable>(info.This());
    v8::Local<v8::Value> val = info[0];
    if (!val->IsUint32()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a non-negative integer", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int v = static_cast<int>(val->IntegerValue(isolate->GetCurrentContext()).ToChecked());
    int v1, v2;
    int retval = nc_inq_var_deflate(obj->parent_id, obj->id, &v1, &v2, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    retval = nc_def_var_deflate(obj->parent_id, obj->id, v1, v2, v);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Variable::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Variable]", v8::NewStringType::kNormal).ToLocalChecked());
}
}  // namespace netcdf4js
