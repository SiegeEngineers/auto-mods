# Build instructions

## Dependencies
### MSYS2 (MingW64) on Windows
Download and follow the install instructions for [MSYS2](https://www.msys2.org/)
  - if you have [Chocolatey](chocolatey.org/) installed you can install MSYS2 as follows

    `choco install msys2`

After installing start a MSYS2-shell from `<MSYS2-directory>/msys2_shell.cmd`

Update the preinstalled packages: `pacman -Syuu`

Install gcc: `pacman -Syu --needed mingw-w64-x86_64-gcc `

And these dependencies:
`pacman -Syu --needed mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-libiconv mingw-w64-x86_64-boost mingw-w64-x86_64-zlib`

### WSL (Ubuntu) on Windows

Please [install WSL on your Windows 10 System](https://docs.microsoft.com/en-us/windows/wsl/install-win10), install the Ubuntu distribution from the Microsoft Store and continue with [Ubuntu](#ubuntu)

### Ubuntu

```
sudo apt update 
sudo apt install --fix-missing gcc g++ cmake \
libboost-iostreams-dev libboost-program-options-dev \
zlib1g-dev
```

## Building

As every other CMake project:

```
./patchGenieutils.sh
mkdir build
cd build
cmake ..
cmake --build .
```
