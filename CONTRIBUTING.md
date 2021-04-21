## Contributing Guidelines

### General

* Fork the repo to your profile.
* Make any changes in your forked repository.
* All changes should be made in the 'main' branch. Do not create a new branch.
* Make a pull request to this main repository.

### Sample Projects

Add all sample projects in the path 
`/Samples/<your-sample-project-directory>/<your-c-file>.c`.

There may be multiple users with their own sample file for the 
sample projects. Do not edit the files contributed by other users.
If naming conflict arises, append your username as a part of your 
filename.

#### List of sample projects you can contribute

- Cellular Automaton 
- 2D Particle Systems
- Basic Interpreter (calculator with memory)
- Fractals
- Tetris
- Basic HTTP Server
- Compression + Decompression of text and binary files using Huffman Encoding (or other compression algorithm of choice)

#### Sample Project Guidelines

* Make use of small elegant libraries that are cross-platform.
* We recommend a single source file (and a few if absolutely necessary).
* Be sure to put instructions about downloading and installing the required libraries, link to relevant sites and you user names at the top of your source file as comments. An example format -

```cpp
/*
 * Libraries
 * SDL2 (link) - zlib license
 * other libraries
*/

// Authors: name (github handle), name (github handle)

/*
 * Project Name
small desc about the project and wiki links and such
*/
```

#### Build Instructions

These are general guidelines you can follow to keep the build 
process consistent across all systems.

Create build.bat (Windows) and build.sh (Linux) that contains the 
build instructions for your program depending upon the system. The
build file should be placed in the path - 
`./Samples/<your-sample-project-directory>/bin/<build-script>`

Please follow the following guidelines as much as possible when 
creating a build file :

- The build files should detect the compiler and properly tell the user if the compiler is not present in the system.
- The build files should take input to all the variables required for the build process.
- The output of the files should be placed in directory `bin` relative to the local project directory.
- All the bin directories (including object files) are to be ignored in the repo.

### README.md

You may contribute by adding to the list of resources. You can also
make grammatical corrections. Create a new category if needed and 
explain in short about the new category/resource in your pull 
request description.
