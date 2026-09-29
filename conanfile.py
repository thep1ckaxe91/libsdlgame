from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.files import copy
import os

class LibSdlGameConan(ConanFile):
    name = "libsdlgame"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {
        "shared": False, 
        "fPIC": True,
        "sdl/*:wayland": False,
        "sdl/*:pulseaudio": False
    }
    
    exports_sources = "CMakeLists.txt", "Config.cmake.in", "src/*", "include/*", "tests/*", "assets/*"

    def requirements(self):
        self.requires("sdl/2.32.10")
        self.requires("sdl_ttf/2.24.0")
        self.requires("sdl_image/2.8.8")
        self.requires("sdl_mixer/2.8.1")
        self.test_requires("gtest/1.17.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["GameEngine"]
        self.cpp_info.set_property("cmake_target_name", "GameEngine::GameEngine")
        # Ensure consumers can include the headers correctly based on how they were installed
        self.cpp_info.includedirs = ["include", "include/libsdlgame"]
