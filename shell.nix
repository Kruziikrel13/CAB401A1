{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc

    vulkan-headers
    vulkan-loader
    vulkan-validation-layers
    vulkan-tools

    clang-tools
    cmake
    spdlog
  ];

  VULKAN_SDK = "${pkgs.vulkan-headers}";
}
