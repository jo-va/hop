## Hop

Simple Monte-Carlo path tracer in C++.
This is a work in progress, only done on my spare time.

[![Build status](https://travis-ci.org/jo-va/hop.svg?branch=master)](https://travis-ci.org/jo-va/hop)

## Features
- OBJ model loading
- Instancing
- Data driven configuration via Lua
- Interactive rendering
- Depth of field

Since I am developping on Linux, the code is targeted to Linux platforms for now, but supporting Windows/Mac OS should not be too difficult.

## Compiling and running
Requires cmake and OpenMP for multithreading, Lua is built along with the project.

Download & Compile:
```
$ git clone https://github.com/jo-va/hop.git
$ cd hop
$ mkdir build
$ cd build
$ cmake ..
$ make
```

Run:
```
$ ./hop scene.lua
```

## Sample Images

Image of a tree, rendered with ambient occlusion.
![Tree](doc/images/tree.png?raw=true "Tree")

Image of a splash, rendered with ambient occlusion and some depth of field.
![Splash AO](doc/images/splash_ao.png?raw=true "Splash AO")
The model was dowloaded on Free3D and is from DOSH Design.

This image shows the ray tracing of 3 mesh instances for a total of 4.5M triangles. It also shows some depth of field.
![Skull](doc/images/skull.png?raw=true "Skull")
I really don't remember where I downloaded this model...

