#include "StructParser.h"


size_t getPrimitiveTypeSize(const std::string& typeName) {
    static const std::unordered_map<std::string, size_t> typeSizes = {
        {"char", 1},
        {"unsigned char", 1},
        {"int8_t", 1},
        {"uint8_t", 1},
        {"short", 2},
        {"unsigned short", 2},
        {"int16_t", 2},
        {"uint16_t", 2},
        {"wchar_t", 2},
        {"int", 4},
        {"unsigned", 4},
        {"unsigned int", 4},
        {"int32_t", 4},
        {"uint32_t", 4},
        {"float", 4},
        {"long", 8},
        {"unsigned long", 8},
        {"long long", 8},
        {"unsigned long long", 8},
        {"int64_t", 8},
        {"uint64_t", 8},
        {"double", 8}
    };

    auto it = typeSizes.find(typeName);
    return (it != typeSizes.end()) ? it->second : 0; //get value or 0
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
            info.dimensions.push_back(1); // reinforcement code added by AI
        }
    }

    return info;
}



void processStruct(const cppast::CppCompound &compoundSymbol)
{
    if (compoundSymbol.compoundType() != cppast::CppCompoundType::STRUCT &&
        compoundSymbol.compoundType() != cppast::CppCompoundType::CLASS) {
        return;
    }

    const std::string structName = compoundSymbol.name();
    StructFieldMap* fieldMap = nullptr;
    if (structName == "SPageFilePhysics") {
        fieldMap = &util.physicsFieldMap;
    } else if (structName == "SPageFileStatic") {
        fieldMap = &util.staticFieldMap;
    } else {
        return;
    }

    size_t currentOffset = 0;

    compoundSymbol.visitAll(
    [&](const cppast::CppEntity& entity)
    {
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
        (*fieldMap)[fieldName] = metadata;

        currentOffset = fieldOffset + fieldSize;
        return true;
    });

    


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

bool initParser(const std::string& filename)
{
    cppparser::CppParser parser;

    std::unique_ptr<cppast::CppCompound> ast(parser.parseFile(filename.c_str()));

    if (!ast) {
        std::cerr << "Failed to parse file: " << filename << std::endl;
        return false;
    }

    traverseAST(*ast);
    return true;
}

