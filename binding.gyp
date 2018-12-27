{
  "targets": [
    {
      "target_name": "urdna2015",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "xcode_settings": { "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
      },
      "msvs_settings": {
        "VCCLCompilerTool": { "ExceptionHandling": 1 },
      },
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "xcode_settings": { "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7",
      },
      "msvs_settings": {
        "VCCLCompilerTool": { "ExceptionHandling": 1 },
      },
      "sources": [
        "lib/native/IdentifierIssuer.cc",
        "lib/native/MessageDigest.cc",
        "lib/native/NQuads.cc",
        "lib/native/addon.cc",
        "lib/native/urdna2015.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "<!@(node -p \"require('node-addon-api').include\")","<!(node -e \"require('nan')\")"],
      "cflags": [
        "-Wno-maybe-uninitialized",
        "-std=c++11"
      ],
      "variables": {
        "node_shared_openssl%": "true"
      },
      "conditions": [
        ['node_shared_openssl=="false"', {
          "include_dirs": [
            "<!@(node -p \"require('node-addon-api').include\")",
            "<!@(node -p \"require('node-addon-api').include\")",
            "<(node_root_dir)/include/node/openssl"
          ]
        }]
      ]
    }
  ]
}
