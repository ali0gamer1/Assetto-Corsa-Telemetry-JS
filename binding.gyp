{
  "targets": [
    {
      "target_name": "addon",
      "sources": [
        "addon.cpp",
        "StructParser.cpp",
        "external/cppparser/cppast/src/cppast.cpp",
        "external/cppparser/cppast/src/cpp_attribute_specifier_sequence_container.cpp",
        "external/cppparser/cppast/src/cpp_blob.cpp",
        "external/cppparser/cppast/src/cpp_compound.cpp",
        "external/cppparser/cppast/src/cpp_control_blocks.cpp",
        "external/cppparser/cppast/src/cpp_entity_info_accessor.cpp",
        "external/cppparser/cppast/src/cpp_enum.cpp",
        "external/cppparser/cppast/src/cpp_expression.cpp",
        "external/cppparser/cppast/src/cpp_function.cpp",
        "external/cppparser/cppast/src/cpp_lambda.cpp",
        "external/cppparser/cppast/src/cpp_templatable_entity.cpp",
        "external/cppparser/cppast/src/cpp_template_param.cpp",
        "external/cppparser/cppast/src/cpp_var_type.cpp",
        "external/cppparser/cppparser/src/cppparser.cpp",
        "external/cppparser/cppparser/src/cpp_program.cpp",
        "external/cppparser/cppparser/src/lexer-helper.cpp",
        "external/cppparser/cppparser/src/parser.lex.cpp",
        "external/cppparser/cppparser/src/parser.tab.cpp",
        "external/cppparser/cppparser/src/utils.cpp"
      ],

      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "external/cppparser/cppast/include",
        "external/cppparser/cppparser/include",
        "external/cppparser/cppparser/src",
        "external/common",
        "external"
      ],

      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],

      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],

      "defines": [ "NAPI_CPP_EXCEPTIONS" ],

      "msvs_settings": {
        "VCCLCompilerTool": {
          "AdditionalOptions": ["/std:c++20", "/Zc:strictStrings-"],
          "ExceptionHandling": 1
        }
      }
    }
  ]
} # type: ignore
