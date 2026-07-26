#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <memory>
#include <sstream>
#include <cctype>

// Include the public headers from the satya-das/cppparser library
#include "cppparser/cppparser.h"

// -----------------------------------------------------------------------------
// Metadata Data Structures
// -----------------------------------------------------------------------------

struct TypeInfo {
    std::string name;
    size_t baseSize = 0;
    std::vector<size_t> dimensions;

    size_t getTotalSize() const {
        if (dimensions.empty()) {
            return baseSize;
        }

        size_t totalElements = 1;
        for (size_t dim : dimensions) {
            totalElements *= dim;
        }
        return baseSize * totalElements;
    }

    std::string getTypeString() const {
        std::string result = name;
        for (size_t dim : dimensions) {
            result += "[" + std::to_string(dim) + "]";
        }
        return result;
    }
};

struct FieldMetadata {
    TypeInfo typeData;
    size_t offset = 0;
};

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

size_t getPrimitiveTypeSize(const std::string& typeName) {
    static const std::unordered_map<std::string, size_t> typeSizes = {
        {"char", 1},
        {"short", 2},
        {"int", 4},
        {"long", 8},
        {"float", 4},
        {"double", 8},
        {"bool", 1},
        {"unsigned", 4},
        {"unsigned int", 4},
        {"unsigned long", 8},
        {"long long", 8},
        {"unsigned long long", 8}
    };

    auto it = typeSizes.find(typeName);
    return (it != typeSizes.end()) ? it->second : 0;
}

size_t getTypeAlignment(const std::string& typeName) {
    auto size = getPrimitiveTypeSize(typeName);
    if (size == 0) {
        return 1;
    }
    return size >= 8 ? 8 : size;
}

size_t alignUp(size_t value, size_t alignment) {
    if (alignment <= 1) {
        return value;
    }
    return ((value + alignment - 1) / alignment) * alignment;
}

// Extract base type and array dimensions from the parser AST
static size_t extractArraySize(const cppast::CppExpression& expr) {
    if (expr.expressionType() != cppast::CppExpressionType::ATOMIC) {
        return 0;
    }

    const auto& atomic = static_cast<const cppast::CppAtomicExpr&>(expr);
    if (atomic.atomicExpressionType() != cppast::CppAtomicExprType::NUMBER_LITEREL) {
        return 0;
    }

    const auto& number = static_cast<const cppast::CppNumberLiteralExpr&>(expr);
    std::stringstream stream(number.value());
    size_t value = 0;
    stream >> value;
    return stream.fail() ? 0 : value;
}

TypeInfo parseType(const cppast::CppVar& var) {
    TypeInfo info;
    info.name = var.varType().baseType();
    info.baseSize = getPrimitiveTypeSize(info.name);

    for (const auto& dimExpr : var.arraySizes()) {
        if (!dimExpr) {
            continue;
        }

        size_t dimValue = extractArraySize(*dimExpr);
        if (dimValue > 0) {
            info.dimensions.push_back(dimValue);
        } else {
            info.dimensions.push_back(1);
        }
    }

    return info;
}

// -----------------------------------------------------------------------------
// AST Visitor / Inspector
// -----------------------------------------------------------------------------

void processStruct(const cppast::CppCompound& compoundSymbol) {
    if (compoundSymbol.compoundType() != cppast::CppCompoundType::STRUCT &&
        compoundSymbol.compoundType() != cppast::CppCompoundType::CLASS) {
        return;
    }

    std::cout << "Found the struct: " << compoundSymbol.name() << "\n";

    std::unordered_map<std::string, FieldMetadata> fieldsMetadata;
    size_t currentOffset = 0;

    compoundSymbol.visitAll([&](const cppast::CppEntity& entity) {
        if (entity.entityType() != cppast::CppEntityType::VAR) {
            return true;
        }

        const auto& var = static_cast<const cppast::CppVar&>(entity);
        std::string fieldName = var.name();

        TypeInfo typeInfo = parseType(var);

        FieldMetadata metadata;
        metadata.typeData = typeInfo;

        size_t fieldSize = typeInfo.getTotalSize();
        size_t fieldAlignment = getTypeAlignment(typeInfo.name);
        size_t fieldOffset = alignUp(currentOffset, fieldAlignment);

        metadata.offset = fieldOffset;
        fieldsMetadata[fieldName] = metadata;

        currentOffset = fieldOffset + fieldSize;
        return true;
    });

    std::cout << "{\n";
    for (const auto& [name, meta] : fieldsMetadata) {
        std::cout << "  \"" << name << "\": { "
                  << "type: \"" << meta.typeData.getTypeString() << "\", "
                  << "size: " << meta.typeData.getTotalSize() << " bytes, "
                  << "offset: " << meta.offset << " }\n";
    }
    std::cout << "}\n\n";
}

void traverseAST(const cppast::CppCompound& program) {
    program.visitAll([&](const cppast::CppEntity& entity) {
        if (entity.entityType() == cppast::CppEntityType::COMPOUND) {
            const auto& compound = static_cast<const cppast::CppCompound&>(entity);
            processStruct(compound);
        }
        return true;
    });
}

// -----------------------------------------------------------------------------
// Main Entry Point
// -----------------------------------------------------------------------------

int main(int argc, char** argv) {
    std::string filename = (argc > 1) ? argv[1] : "AcPhysics.cpp";

    cppparser::CppParser parser;

    std::unique_ptr<cppast::CppCompound> ast(parser.parseFile(filename.c_str()));

    if (!ast) {
        std::cerr << "Failed to parse file: " << filename << std::endl;
        return 1;
    }

    traverseAST(*ast);

    return 0;
}