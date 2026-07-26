#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct TypeInfo
{
    std::string name;
    size_t baseSize = 0;
    std::vector<size_t> dimensions;

    size_t getTotalSize() const
    {
        if (dimensions.empty())
            return baseSize;

        size_t totalElements = 1;
        for (size_t dim : dimensions) {
            totalElements *= dim;
        }
        return baseSize * totalElements;
    }

    std::string getTypeString() const
    {
        std::string result = name;
        for (size_t dim : dimensions) {
            result += "[" + std::to_string(dim) + "]";
        }
        return result;
    }
};

struct FieldMetadata
{
    TypeInfo typeData;
    size_t offset = 0;
};

using StructFieldMap = std::unordered_map<std::string, FieldMetadata>;

struct UtilStruct
{
    void* graphicHmap = nullptr;
    void* staticHmap = nullptr;
    void* physicsHmap = nullptr;
    
    void* physics = nullptr;
    void* staticPtr = nullptr;
    StructFieldMap physicsFieldMap;
    StructFieldMap staticFieldMap;
};

inline UtilStruct util;