{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  name = "CAB401 A1 Dev Shell";
  buildInputs = with pkgs; [
    gcc
    cmake
    glslang
    shader-slang
    valgrind
    gdb

    llvmPackages_21.openmp

    ## LSP
    neocmakelsp
    glsl_analyzer
    clang-tools

    ## Libs
    vulkan-headers
    vulkan-loader
    vulkan-tools
    vulkan-tools-lunarg
    vulkan-validation-layers
    spdlog
  ];

  LD_LIBRARY_PATH = "${pkgs.vulkan-loader}/lib:${pkgs.vulkan-validation-layers}/lib";
  VULKAN_SDK = "${pkgs.vulkan-headers}";
  VK_LAYER_PATH = "${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d";
}
