
#include <napi.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <type_traits>
#include "AcPhysics.h"
#include "StructParser.h"
#include "AcStatic.h"
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

static std::string wideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty())
        return {};

    int requiredSize = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (requiredSize <= 0)
        return {};

    std::string result(requiredSize, '\0');
    ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), result.data(), requiredSize, nullptr, nullptr);
    return result;
}

template<typename T>
std::string scalarValueToString(void* base, std::size_t offset)
{
    T value = readAtOffset<T>(base, offset);

    if constexpr (std::is_same_v<T, bool>)
    {
        return value ? "true" : "false";
    }
    else if constexpr (std::is_same_v<T, char>)
    {
        return std::string(1, value);
    }
    else if constexpr (std::is_same_v<T, wchar_t>)
    {
        return wideToUtf8(std::wstring(1, value));
    }
    else
    {
        return std::to_string(value);
    }
}

template<typename T>
std::string arrayValueToString(void* base, std::size_t offset, std::size_t elementCount)
{
    std::ostringstream oss;

    if constexpr (std::is_same_v<T, wchar_t>)
    {
        for (std::size_t index = 0; index < elementCount; ++index)
        {

            T value = readAtOffset<T>(base, offset + index * sizeof(T));

            oss << wideToUtf8(std::wstring(1, value));
        }

        return oss.str();
    }


    oss << '[';

    for (std::size_t index = 0; index < elementCount; ++index)
    {
        if (index > 0)
            oss << ',';

        T value = readAtOffset<T>(base, offset + index * sizeof(T));

        if constexpr (std::is_same_v<T, bool>)
        {
            oss << (value ? "true" : "false");
        }
        else if constexpr (std::is_same_v<T, char>)
        {
            oss << '\'' << value << '\'';
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            oss << '"' << wideToUtf8(std::wstring(1, value)) << '"';
        }
        else
        {
            oss << value;
        }
    }

    oss << ']';
    return oss.str();
}

using ScalarReader = std::string (*)(void*, std::size_t);
using ArrayReader = std::string (*)(void*, std::size_t, std::size_t);

struct TypeReader
{
    ScalarReader scalar;
    ArrayReader array;
};

static const std::unordered_map<std::string, TypeReader> kTypeReaders = {
    {"int", {scalarValueToString<int>, arrayValueToString<int>}},
    {"unsigned int", {scalarValueToString<unsigned int>, arrayValueToString<unsigned int>}},
    {"float", {scalarValueToString<float>, arrayValueToString<float>}},
    {"double", {scalarValueToString<double>, arrayValueToString<double>}},
    {"bool", {scalarValueToString<bool>, arrayValueToString<bool>}},
    {"char", {scalarValueToString<char>, arrayValueToString<char>}},
    {"wchar_t", {scalarValueToString<wchar_t>, arrayValueToString<wchar_t>}},
    {"short", {scalarValueToString<short>, arrayValueToString<short>}},
    {"unsigned short", {scalarValueToString<unsigned short>, arrayValueToString<unsigned short>}},
    {"long", {scalarValueToString<long>, arrayValueToString<long>}},
    {"unsigned long", {scalarValueToString<unsigned long>, arrayValueToString<unsigned long>}},
    {"long long", {scalarValueToString<long long>, arrayValueToString<long long>}},
    {"unsigned long long", {scalarValueToString<unsigned long long>, arrayValueToString<unsigned long long>}},
    {"int8_t", {scalarValueToString<int8_t>, arrayValueToString<int8_t>}},
    {"uint8_t", {scalarValueToString<uint8_t>, arrayValueToString<uint8_t>}},
    {"int16_t", {scalarValueToString<int16_t>, arrayValueToString<int16_t>}},
    {"uint16_t", {scalarValueToString<uint16_t>, arrayValueToString<uint16_t>}},
    {"int32_t", {scalarValueToString<int32_t>, arrayValueToString<int32_t>}},
    {"uint32_t", {scalarValueToString<uint32_t>, arrayValueToString<uint32_t>}},
    {"int64_t", {scalarValueToString<int64_t>, arrayValueToString<int64_t>}},
    {"uint64_t", {scalarValueToString<uint64_t>, arrayValueToString<uint64_t>}}
};

Napi::String GetValue(const Napi::CallbackInfo &args)
{
    Napi::Env env = args.Env();

    if (args.Length() != 2)
    {
        Napi::TypeError::New(env, "Expected struct name and field key").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    

    std::string structName = args[0].ToString().Utf8Value();
    std::string fieldName = args[1].ToString().Utf8Value();

    void* base = nullptr;
    if (structName == "static")
    {
        base = util.staticPtr;
    }
    else if (structName == "physics")
    {
        base = util.physics;
    }
    else
    {
        Napi::TypeError::New(env, "Unknown struct name").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    if (!base)
    {
        Napi::TypeError::New(env, "Target struct is not initialized").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    const StructFieldMap* fieldMap = nullptr;
    if (structName == "static")
    {
        fieldMap = &util.staticFieldMap;
    }
    else if (structName == "physics")
    {
        fieldMap = &util.physicsFieldMap;
    }

    if (!fieldMap)
    {
        Napi::TypeError::New(env, "Field map not available for requested struct").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    auto it = fieldMap->find(fieldName);
    if (it == fieldMap->end())
    {
        Napi::TypeError::New(env, "Field not found").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    const auto& meta = it->second;
    const auto& typeInfo = meta.typeData;
    const auto& typeName = typeInfo.name;

    auto elementCount = std::size_t{1};
    for (size_t dim : typeInfo.dimensions)
        elementCount *= dim;

    auto readerIt = kTypeReaders.find(typeName);
    if (readerIt == kTypeReaders.end())
    {
        Napi::TypeError::New(env, "Unsupported type").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    const TypeReader& reader = readerIt->second;
    std::cout << "[DEBUG] GetValue struct='" << structName << "' field='" << fieldName
              << "' base=" << base << " offset=" << meta.offset
              << " type='" << typeName << "' dimensions=" << typeInfo.dimensions.size()
              << " elementCount=" << elementCount << "\n";

    if (!typeInfo.dimensions.empty())
    {
        std::cout << "[DEBUG] dimensions:";
        for (size_t dim : typeInfo.dimensions)
            std::cout << " " << dim;
        std::cout << '\n';
    }

    std::string valueString;

    if (typeInfo.dimensions.empty())
    {
        valueString = reader.scalar(base, meta.offset);
    }
    else
    {
        valueString = reader.array(base, meta.offset, elementCount);
    }

    std::cout << "[DEBUG] resolved value='" << valueString << "'\n";
    return Napi::String::New(env, valueString);
}



Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    
    util.physicsHmap = OpenFileMappingA(
        FILE_MAP_READ,
        FALSE,
        "Local\\acpmf_physics");
    util.physics = static_cast<SPageFilePhysics*>(
        MapViewOfFile(
            util.physicsHmap,
            FILE_MAP_READ,
            0,
            0,
            sizeof(SPageFilePhysics)));


    util.staticHmap = OpenFileMappingA(
        FILE_MAP_READ,
        FALSE,
        "Local\\acpmf_static");

    util.staticPtr = static_cast<SPageFileStatic*>(
        MapViewOfFile(
            util.staticHmap,
            FILE_MAP_READ,
            0,
            0,
            sizeof(SPageFileStatic)));
    
    initParser("AcPhysics.h");
    initParser("AcStatic.h");
    

    exports.Set("GetValue",
    Napi::Function::New(env, GetValue)
    );

    return exports;
}

NODE_API_MODULE(addon, Init)