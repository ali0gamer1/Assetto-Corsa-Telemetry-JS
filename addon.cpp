
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



Napi::String GetValue(const Napi::CallbackInfo &args)
{
    Napi::Env env = args.Env();

    if (args.Length() != 1)
    {
        Napi::TypeError::New(env, "Expected one argument").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    initParser();

    return args[0].ToString();
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