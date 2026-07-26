
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



    std::string fieldName = args[0].ToString().Utf8Value();
    auto it = util.fieldMap.find(fieldName);

    if (it == util.fieldMap.end())
    {
        Napi::TypeError::New(env, "Field not found").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    const auto& meta = it->second;
    const auto& typeInfo = meta.typeData;
    const std::string& typeName = typeInfo.name;
    std::string valueString;

    auto elementCount = size_t{1};
    for (size_t dim : typeInfo.dimensions)
        elementCount *= dim;

    auto serializeArray = [&](auto typeSample) {
        using T = decltype(typeSample);
        std::ostringstream oss;
        oss << '[';

        for (size_t index = 0; index < elementCount; ++index)
        {
            if (index > 0)
                oss << ',';

            T value = readAtOffset<T>(util.physics, meta.offset + index * sizeof(T));
            if constexpr (std::is_same_v<T, bool>)
            {
                oss << (value ? "true" : "false");
            }
            else if constexpr (std::is_same_v<T, char>)
            {
                oss << '\'' << value << '\'';
            }
            else
            {
                oss << value;
            }
        }

        oss << ']';
        return oss.str();
    };

    if (typeInfo.dimensions.empty())
    {
        if (typeName == "int")
        {
            valueString = std::to_string(readAtOffset<int>(util.physics, meta.offset));
        }
        else if (typeName == "unsigned int")
        {
            valueString = std::to_string(readAtOffset<unsigned int>(util.physics, meta.offset));
        }
        else if (typeName == "float")
        {
            valueString = std::to_string(readAtOffset<float>(util.physics, meta.offset));
        }
        else if (typeName == "double")
        {
            valueString = std::to_string(readAtOffset<double>(util.physics, meta.offset));
        }
        else if (typeName == "bool")
        {
            bool value = readAtOffset<bool>(util.physics, meta.offset);
            valueString = value ? "true" : "false";
        }
        else if (typeName == "char")
        {
            valueString = std::string(1, readAtOffset<char>(util.physics, meta.offset));
        }
        else if (typeName == "short")
        {
            valueString = std::to_string(readAtOffset<short>(util.physics, meta.offset));
        }
        else if (typeName == "unsigned short")
        {
            valueString = std::to_string(readAtOffset<unsigned short>(util.physics, meta.offset));
        }
        else if (typeName == "long")
        {
            valueString = std::to_string(readAtOffset<long>(util.physics, meta.offset));
        }
        else if (typeName == "unsigned long")
        {
            valueString = std::to_string(readAtOffset<unsigned long>(util.physics, meta.offset));
        }
        else if (typeName == "long long")
        {
            valueString = std::to_string(readAtOffset<long long>(util.physics, meta.offset));
        }
        else if (typeName == "unsigned long long")
        {
            valueString = std::to_string(readAtOffset<unsigned long long>(util.physics, meta.offset));
        }
        else if (typeName == "int8_t")
        {
            valueString = std::to_string(readAtOffset<int8_t>(util.physics, meta.offset));
        }
        else if (typeName == "uint8_t")
        {
            valueString = std::to_string(readAtOffset<uint8_t>(util.physics, meta.offset));
        }
        else if (typeName == "int16_t")
        {
            valueString = std::to_string(readAtOffset<int16_t>(util.physics, meta.offset));
        }
        else if (typeName == "uint16_t")
        {
            valueString = std::to_string(readAtOffset<uint16_t>(util.physics, meta.offset));
        }
        else if (typeName == "int32_t")
        {
            valueString = std::to_string(readAtOffset<int32_t>(util.physics, meta.offset));
        }
        else if (typeName == "uint32_t")
        {
            valueString = std::to_string(readAtOffset<uint32_t>(util.physics, meta.offset));
        }
        else if (typeName == "int64_t")
        {
            valueString = std::to_string(readAtOffset<int64_t>(util.physics, meta.offset));
        }
        else if (typeName == "uint64_t")
        {
            valueString = std::to_string(readAtOffset<uint64_t>(util.physics, meta.offset));
        }
        else
        {
            valueString = typeName;
        }
    }
    else
    {
        if (typeName == "int")
        {
            valueString = serializeArray(int{});
        }
        else if (typeName == "unsigned int")
        {
            valueString = serializeArray(unsigned int{});
        }
        else if (typeName == "float")
        {
            valueString = serializeArray(float{});
        }
        else if (typeName == "double")
        {
            valueString = serializeArray(double{});
        }
        else if (typeName == "bool")
        {
            valueString = serializeArray(bool{});
        }
        else if (typeName == "char")
        {
            valueString = serializeArray(char{});
        }
        else if (typeName == "short")
        {
            valueString = serializeArray(short{});
        }
        else if (typeName == "unsigned short")
        {
            valueString = serializeArray(unsigned short{});
        }
        else if (typeName == "long")
        {
            valueString = serializeArray(long{});
        }
        else if (typeName == "unsigned long")
        {
            valueString = serializeArray(unsigned long{});
        }
        else if (typeName == "long long")
        {
            valueString = serializeArray(long long{});
        }
        else if (typeName == "unsigned long long")
        {
            valueString = serializeArray(unsigned long long{});
        }
        else if (typeName == "int8_t")
        {
            valueString = serializeArray(int8_t{});
        }
        else if (typeName == "uint8_t")
        {
            valueString = serializeArray(uint8_t{});
        }
        else if (typeName == "int16_t")
        {
            valueString = serializeArray(int16_t{});
        }
        else if (typeName == "uint16_t")
        {
            valueString = serializeArray(uint16_t{});
        }
        else if (typeName == "int32_t")
        {
            valueString = serializeArray(int32_t{});
        }
        else if (typeName == "uint32_t")
        {
            valueString = serializeArray(uint32_t{});
        }
        else if (typeName == "int64_t")
        {
            valueString = serializeArray(int64_t{});
        }
        else if (typeName == "uint64_t")
        {
            valueString = serializeArray(uint64_t{});
        }
        else
        {
            valueString = typeName + "[]";
        }
    }

    return Napi::String::New(env, valueString);

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