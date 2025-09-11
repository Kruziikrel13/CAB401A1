{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    cmake
    glslang

    ## LSP
    neocmakelsp
    glsl_analyzer
    clang-tools

    ## Libs
    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    spdlog
  ];

  VULKAN_SDK = "${pkgs.vulkan-headers}";
}
