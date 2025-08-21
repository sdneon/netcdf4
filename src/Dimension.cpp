#include "Dimension.h"
#include <netcdf.h>
#include "netcdf4js.h"

namespace netcdf4js {

v8::Persistent<v8::Function> Dimension::constructor;

Dimension::Dimension(const int& id_, const int& parent_id_) : id(id_), parent_id(parent_id_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
}

void Dimension::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Dimension", v8::NewStringType::kNormal).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Dimension::Inspect);

    FN_GETTER(Dimension, Id)
    FN_GETTER(Dimension, Length)
    FN_GETTER_AND_SETTER(Dimension, Name)

    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "id", v8::NewStringType::kNormal).ToLocalChecked(), getter_Id);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked(), getter_Length);
    tpl->InstanceTemplate()->SetAccessorProperty(v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), getter_Name, setter_Name);
    constructor.Reset(isolate, tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

bool Dimension::get_name(char* name) const {
    int retval = nc_inq_dimname(parent_id, id, name);
    if (retval != NC_NOERR) {
        throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

void Dimension::GetId(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.This());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
}

size_t Dimension::getLen()
{
    size_t len;
    int retval = nc_inq_dimlen(this->parent_id, this->id, &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return -1;
    }
    return len;
}

void Dimension::GetLength(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.This());
    size_t len;
    int retval = nc_inq_dimlen(obj->parent_id, obj->id, &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    info.GetReturnValue().Set(v8::Integer::New(isolate, len));
}

void Dimension::GetName(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.This());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked());
    }
}

void Dimension::SetName(const v8::FunctionCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.This());
    v8::Local<v8::Value> val = info[0];
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int retval = nc_rename_dim(obj->parent_id, obj->id, *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
}

void Dimension::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Dimension]", v8::NewStringType::kNormal).ToLocalChecked());
}
}  // namespace netcdf4js
