Two examples of basic arithmetic expressions with visitor for their evaluation. The first is python implementation stored in `simple_expressions.py` file and should work with any reasonably fresh Python version. The second one is written in C++ and uses `static_variant` from fc library provided by the Hive blockchain. To compile it:
 1. make sure that the submodules are downloaded with `git submodule update --init --recursive`,
 2. configure the project with CMake: `cmake .` and
 3. compile it with `make` (preferably `make -j8` as it takes some time to compile the dependencies).
