# MakeGen
MakeGen is a tool to generate Makefiles in a simpler manner than creating them yourself.
But it also generates Makefiles so that sources files that depend on header files actually compile if the header files changes.
This is also done recursivly so if header files depend on other header files these will also be added as a dependency.
This means that there wont be any weird compiler errors when you've changed things in a header file.
Which could sometimes force you to rebuild the entire project.

## Supported platforms
Currently only Linux is supported.
It has been tested under both Ubuntu and Manjaro.
But shouldn't really be a problem for other Linux distros (otherwise submit an issue and I might take a look at it).

## Dependencies
To compile and run MakeGen the only requirement is `make` and `g++`.
After MakeGen is installed it will always be able to generate a Makefile (even without `make` or `g++` installed).
However it will not compile your code if those programs don't exist.

## Installation
To install MakeGen make sure you have the dependencies listed above.
Then clone this repository:

    git clone git@github.com:Thraix/MakeGen.git

Then navigate into the MakeGen folder (`cd MakeGen`) and run:

    sudo make install

This will install MakeGen into `/usr/bin/makegen`.

If you want it to be installed in another place just run `make` and copy the file to where you want it.

**NOTE**: If you are installing from the latest commit, debug flags might be enabled for the project. As such, MakeGen can run slightly slower. To resolve this edit the Makefile and remove `-D_DEBUG -g3 -w` from the `CFLAGS` variable. This should not be a problem in released versions after version `1.3.0`.

## Usage
### Config file
In order to create a MakeGen configuration file use the following command:

    makegen conf gen prompt

This will prompt you with all the needed configurations in order to create a Makefile which will compile your code.
If you want to create a quick and simple config file you can also run:

    makegen conf gen default

Which will default the source directory to `src/`, output directory to `bin/` and project name to the current directory name.

When this is done it will create a file called `makegen.conf` which contains all relevant data for MakeGen to create a Makefile.

If you want to change your config you can modify the makegen.conf file or use makegens config command line interface.
To see the possible changes you can do see the help file by entering:

    makegen conf

### Compiling
After that simply compile your code with:

    makegen

This will compile your code using `make all` with the specified libraries linked dynamically.
The compilation will also make use of `make --jobs=X` in order to compile on multiple threads.
The Makefile can also be run using normal make calls, so others compiling your code don't need to install MakeGen.

There are also optional flags that can be given to makegen in order to rebuild, clean, run and install your code.
When installing the project you need to run as sudo since it will try to install the project into /usr/bin/.
It can also run the compilation on a single thread using the `--single` flag.
If multiple options are given to MakeGen it will execute them in the following order:

    clean all install run

Since MakeGen generates include dependencies in your Makefile it can cause the Makefile to get very cluttered.
In order to make it less cluttered you can run `makegen --simple` which will generate (and run) a Makefile without header file dependencies.
