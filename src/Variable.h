#ifndef NETCDF4JS_VARIABLE_H
#define NETCDF4JS_VARIABLE_H

#include <netcdf.h>
#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {

class Variable : public node::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports);
    Variable(const int& id_, const int& parent_id_);
    bool get_name(char* name) const;

  private:
    static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ReadStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void WriteSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void WriteStridedSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetId(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetType(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetDimensions(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetAttributes(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetName(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetName(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetEndianness(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetEndianness(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetChecksumMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetChecksumMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetChunkMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetChunkMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetChunkSizes(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetChunkSizes(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetFillMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetFillMode(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetFillValue(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetFillValue(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetCompressionShuffle(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetCompressionShuffle(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetCompressionDeflate(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetCompressionDeflate(const v8::FunctionCallbackInfo<v8::Value>&);
    static void GetCompressionLevel(const v8::FunctionCallbackInfo<v8::Value>&);
    static void SetCompressionLevel(const v8::FunctionCallbackInfo<v8::Value>&);
    static void Inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
    static const unsigned char type_sizes[];
    static const char* type_names[];
    int id;
    int parent_id;
    nc_type type;
    int ndims;
};
}  // namespace netcdf4js

#endif
