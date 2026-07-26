
#include <napi.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "AcPhysics.h"
#include "StructParser.h"
#include "util.h"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

template<typename T>
T& readAtOffset(void* base, std::size_t offset)
{
    return *reinterpret_cast<T*>(
        reinterpret_cast<uint8_t*>(base) + offset
    );
}

Napi::String GetValue(const Napi::CallbackInfo &args)
{
    Napi::Env env = args.Env();

    if (args.Length() != 1)
    {
        Napi::TypeError::New(env, "Expected one argument").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    initParser();

    // std::cout << "{\n";
    // for (const auto& [name, meta] : util.fieldMap) {
    //     std::cout << "  \"" << name << "\": { "
    //               << "type: \"" << meta.typeData.getTypeString() << "\", "
    //               << "size: " << meta.typeData.getTotalSize() << " bytes, "
    //               << "offset: " << meta.offset << " }\n";
    // }
    // std::cout << "}\n\n";



    auto it = util.fieldMap.find(args[0].ToString().Utf8Value());

    std::string fieldName = args[0].ToString().Utf8Value();

    //perform black magic to get the value by calculating the offset.

    int gear = (readAtOffset<int>(util.physics,it->second.offset));
    std::cout<<gear;
    return Napi::String::New(env,it->second.typeData.name);
}



Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    
    util.hMapFile = OpenFileMappingA(
        FILE_MAP_READ,
        FALSE,
        "Local\\acpmf_physics");
    
    util.physics = static_cast<SPageFilePhysics*>(
        MapViewOfFile(
            util.hMapFile,
            FILE_MAP_READ,
            0,
            0,
            sizeof(SPageFilePhysics)));
    
    exports.Set("GetValue",
    Napi::Function::New(env, GetValue)
    );

    return exports;
}

NODE_API_MODULE(addon, Init)