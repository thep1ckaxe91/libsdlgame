from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain

class MyProjectConan(ConanFile):
    name = "my_project"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    
    requires = [
        "gtest/1.17.0",
        "sdl/2.32.10",
        "sdl_ttf/2.24.0",
        "sdl_image/2.8.8",
        "sdl_mixer/2.8.1"
    ]

    default_options = {
        "sdl/*:wayland": False,
        "pulseaudio/*:shared": True
    }

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()
