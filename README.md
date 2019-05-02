## Build and Run
### Build without install
On Linux (make sure you have cmake 3.13 or above,
C++ compiler support C++17, git, and boost library 1.65.1)
```bash
$ git clone git@github.com:junior-2016/Compiler.git
$ cd Compiler
$ mkdir build
$ cd build
$ cmake ..
$ make 
use make install to install this binary.
```

### Run 
On Linux
```bash 
$ cd build
$ ./Compiler [source file name1] [source file name2] ... 
```
