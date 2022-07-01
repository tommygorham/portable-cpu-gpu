# Build Directions for Using RAJA As A Library in Your C++/C Application 
For now, these are the most basic build commands for serial compilation to get things working

### Clone RAJA
`git clone --recursive https://github.com/llnl/raja.git`

## Configure With CMake and Build

    mkdir build && cd build
 
    cmake .. -DCMAKE_INSTALL_PREFIX=<where-you-want-to-install-raja> 
             -DCMAKE_CXX_COMPILER=g++ 
              -DCMAKE_C_COMPILER=gcc
          
    make install 

Be sure to pass the compiler commands so that cmake doesn't use the systems default (depending on the configuration)
Also, you may need to pass the paththe the compiler, as opposed to just g++/gcc. 

## Now, cd To Your Application and make a CMakeLists.txt

    cd 
    cd myapp
    vim CMakeLists.txt

## Build your application with RAJA
    mkdir build && cd build
    !cmake 
    make 
    ./<your-exe> 
