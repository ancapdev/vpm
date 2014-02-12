# Versioned Package Make
## Introduction
VPM is a framework for managing source level C++ packages and their dependencies, with a goal to provide flexible configuration management optmized for large scale software development. It is not intended for deployment and installation of software to end users, but to help with the composition of software from individually versioned modules, called packages.

The framework is built over `CMake` and constitutes a set of `CMake` scripts that deal with fetching and combining packages into a build, and command line tool (`vpm`) to make the invocation of the framework more convenient.

### Package layout
Packages live in one or more package roots in a structured layout:
```
<package_root>/<package>/<version>[-<variant>]/...
```

Internally a package has one or both of:
- A `configure.cmake` script. This script is included in all dependent packages. It exposes public properties of the package, like include directories, import targets, macros, and variables.
- A `CMakeLists.txt` script. This script is included in the normal `CMake` way to build the package.

### Versions
They way VPM implements configuration managements is through package versions. Versions can be selected globally or directly when specifying dependencies within a package, and only a single version of a particular package is used throughout a single build. From the point of view of the framework, versions are immutable once fetched, and the framework will make no attempt to update existing packages. The suggested way to deal with package upgrades is therefore to maintain immutable release versions, and change the version number when bugs are fixed or new features are added. This is also cleaner and more explict from a package consumer point of view than pulling repositories for changes. For development lines, it is advisable to clone and maintain repos manually.

Typically versions correspond to branches in a git repo. Using tags could be an alternative to branches for release versions, and more clearly communicates immutability. However, tags would require a clone of some branch and subsequent checkout of the tag, whereas branches can be cloned directly. This simplifies the fetch command configuration, and also easily allows for shallow clones of auto-downloaded packages. Tags are currently not supported.

When looking for packages, the framework searches the package roots based on the version and variant specified for each package. These properties determinde the possible physical locations for a package. SCM branches, tags, etc, are never inspected to search for or validates versions.

### Variants
Variants are a way to set up multiple different implementations of a package that all provide the same public interface and functionality. These would typically be used to package up pre-built libraries for different architectures and platforms separately, or to search for those libraries on the system, or even build them from source. Different variants point to different source repositories (potentially in different package roots and/or from different package repositories and hosts), and the repository name is defined as `<package>-<variant>`. Variant names are arbitrary and user defined.

## Build
The vpm framework lives in the package tree, so first create a place to store packages. E.g.,
```Shell
mkdir ~/packages/
```
Now create a directory for the vpm package itself, and clone the framework into a versioned subdirectory.
```Shell
mkdir ~/packages/vpm
git clone git@github.com:ancapdev/vpm -b 1.06.00 ~/packages/vpm/1.06.00
```
Finally, build the `vpm` tool. The framework has 2 build modes, self build, and package build. Its default mode is a self build, so simply run `CMake` the standard way to build `vpm`. Later, the `vpm` tool will take care of invoking CMake with the necessary arguments to run a package build.
```Shell
mkdir ~/build && mkdir ~/build/vpm && cd ~/build/vpm
cmake ~/packages/vpm/1.06.00
make
```

## Install and Configure
The `vpm` tool is a standalone binary with no dependencies. Copy it to your path to use it. E.g.,
```Shell
cp ./vpm ~/bin/
```

The tool captures many of its default options from the environment at build time, such as the framework path, the package roots, the `CMake` generator to use, and the architecture width. These can optionally be overriden by adding a .vpm.yml configuration file to the user home directory. An example config file exists in the framework directory.

## Add package repositories
In order for the framework to find and download packages, one or more repository descriptions must be added to one or more of the package roots. Repository descriptions live in `.repos` subdirectories under the package roots. Typically, _package_ repository descriptions will be _git_ repositories themselves. For example, to add the https://github.com/ancapdev repository, clone `git@github.com:ancapdev/packages.meta` into the `.repos` folder. The name for the repository is arbitrary, but if more than 1 exists, they will be searched in alphabetical order when fetching new packages.
```Shell
mkdir ~/packages/.repos
git clone git@github.com:ancapdev/packages.meta ~/packages/.repos/ancapdev
```
From time to time, as new packages are added, it may be necessary to update the repository description. Do this the same way you would update any other repo. E.g.,
```Shell
cd ~/packages/.repos/ancapdev
git pull
```

## Build packages
A package build can be initiated by invoking `CMake` directly, or through the `vpm` tool. The `vpm` tool isn't strictly required, but it makes passing many of the necessary parameters to `CMake` and the framework much more convenient.

### The vpm tool

`vpm` supports a set of commands with a general syntax:
```Shell
vpm <command> <arguments>
```

#### Help
To get help on a specific command, run the `help` command:
```Shell
vpm help <command>
```
This will provide a description for a command and list all its options.

#### Options
Arguments are either named options or unnamed. Options come in 3 forms:
- Flags: have no value, only `-<key>`.
- Enumerations: `-<key>=<value>`, where value is constrained to a set.
- Strings: `-<key>=<value>`, where value is any string.

Keys can be shortened to their shortest unambiguous key prefix. For example:
```Shell
vpm mbuild crunch.base -what
```
Will run the `mbuild` command with the `-whatif` option.

If enumeration keys can be determined unambiguously from their values, they can be provided by value only through the `+<value>` syntax. For example:
```Shell
vpm mbuild crunch.base +ninja
```
Will run the `mbuild` command with the `-generator=ninja` option.

### The mbuild command
`mbuild` is the meta build command. It's responsible for generating build files from packages by invoking `CMake` on the vpm framework and passsing all the necessary arguments. It takes the following form:
```Shell
vpm mbuild <package1>..<packageN> [options]
```
Where `<package1>..<packageN>` is a list of of packages to build. Each package argument takes the form `<name>[?<variant>][@<version>]`. For example, to generate build files for `crunch.base` version 1.00.00, and all of its dependencies, run:
```Shell
vpm mbuild crunch.base@1.00.00
```
A full list of options can be retrieved by running `vpm help mbuild`.


## Author packages
