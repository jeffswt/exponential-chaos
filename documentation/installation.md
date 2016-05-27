
# Installation

## Brief

Exponential Chaos is provided in two methods, source code and compiled binaries.
Reading this file is very important throughout the experience of the game. Do not
attempt to avoid using this game without reading the manual.

This manual gives instructions of both installation methods. After reading the
manual, you would be able to possess the ability to begin the game from both
methods.

## Install from source

This program is created under the **Eclipse Integrated Development Environment**.
And the source code is shipped with the preconfigured Eclipse configuration. The
standard environment is:

 * Eclipse IDE Mars.1
 * TDM-GCC 4.9.2 (all platforms)

After setting up the environment, you may proceed to the compilation. Related
libraries are required to link the program correctly. The default setting is
Windows 32-bit, while the pre-compiled libraries are designed only to conform
to this platform. The required libraries are:

 * ```FreeGLUT``` (MinGW) 3.0.0
 * ```GLEW``` 1.13.0
 * ```libpng``` 1.2.37 (or newer)
 * ```zlib``` 1.2.3 (or newer)

In order to update the version stamp of a library, you need to take extra
precautions of modifying the headers, while also taking care of the function
invocations inside the source. That is, after modifying a version, the entire
project should be re-compiled to ensure intactness.

If you are not using Eclipse, instead using GNU Make, you would probably have
to write your own make configuration. The include folders are:

 * The standard library headers (STL)
 * ```project_folder/```
 * ```project_folder/include/```

Nevertheless to mention the language dialect is **C++11**.

After the installation, you would have to install the resource packs manually
and the default GUI textures manually. These files can be found in the Release
tab, which is contained in the installation packs.

## Installing from binaries

Under the release tabs, you may find the installation packs useful. If you are
new to Exponential Chaos, you can download the latest full installation pack.
Minor updates are not provided with full installation packs, instead they only
contain the updated files, from the given checkpoint, often the latest full
release.

You would have to manually extract the archives to the program folder root to
perform a quick update. Sometimes full installation packs are not archived
with installers, instead only archives.

Deal with this with precaution.
