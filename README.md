=========
libleeloo
=========

What is it ?
------------

libleeloo is a C++ library (with Python bindings) that help manages intervals
of integers with fast and memory-wise randomisation of their elements. It can
be used directly in any C++ program or through the Python bindings.

One common usage of this library is to aggregate and randomize IPv4 ranges,
which can be found in large-scale IP scanners. See the Examples section for more
information.

Compilation
-----------

You need at least GCC 4.7.3 (4.7.2 crashes) or Clang 3.4, a Linux-based system, and these libraries:

 * Boost Random >= 1.49
 * Intel Threading Building Blocks (https://www.threadingbuildingblocks.org/) >= 4.0
 * Boost Python >= 1.49 for Python bindings (thus not necessary)
 * Boost Serialization >= 1.49 for random state persistance (thus not necessary)

Support for other OS-es and compilers may be provied in the future.

Under Debian-based systems, this can be installed thanks to:

    # aptitude install libtbb-dev libboost-random-dev libboost-python-dev

This project uses CMake. To compile it, follow these steps:

    $ cd /path/to/src
    $ mkdir build && cd build
    $ cmake -DLEELOO_BOOST_SERIALIZE=1 -DCMAKE_BUILD_TYPE=Release ..
    $ make

Use -DLEELOO_BOOST_SERIALIZE=1 if you want to support random state persistance through the ``list_intervals_random*`` classes (needed for [nodescan](https://github.com/quarkslab/nodescan)).

The supported build types are :

 * debug: compiles with no optimisations and debug symbols (-g)
 * relwithdebinfo : compiles with full optimisations and debug symbols (-O3 -march=native -g)
 * release : compiles with full optimisations and no debug symbol (-O3 -march=native)

This will compile the library. Then, as root, you can install it :

    # make install

In order to install the python bindings, do the following :

    $ cd /path/to/src
    $ cd bindings/python
    $ mkdir build && cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release -DPYHON_VERSION=3.3 ..
    $ make

Then, as root :

    # make install

The PYTHON_VERSION variable tells CMake for which Python version the bindings must be compiled.

Installation
------------

After building the library, just run as root:

    # make install

This will install the library to /usr/local, and the Python module to the current Python installation.


Debian packages
---------------

Debian packages can be downloaded on Quarkslab's public debian repository.
Packages for the stable (wheezy) and unstable (sid) versions are available.

Instructions are available [here](http://apt.quarkslab.com/readme.txt) to add this repository to your system.

Then, you simply have to do:

    # apt-get update
    # apt-get install libleeloo

Development packages are also available:

    # apt-get install libleeloo-dev

Python 2.7 and 3 bindings are also available as debian packages:

    # apt-get install python-pyleeloo python3-pyleeloo


Usage examples
--------------

Some examples can be found in the ``tools/`` and ``tests/`` directory.

One tool is provided for randomizing IP ranges :

    $ ./ip_ranges_rand
    Usage: ./ip_ranges_rand range1 [-][range2] [-][range3] ...
    
    This tool will aggregate all the given ranges and will write a random order of the corresponding IP.
    
    where range can be described as:
            CIDR notation:  192.168.1.0/24
            Ranges:         10.4-5.8.9-250
            Single IP:      192.168.1.10
    
    A '-' symbol before any range will remove it from the final set.

You can also read a blog post about some math used in this library in Quarkslab's blog here : http://blog.quarkslab.com/unique-random-number-set-computation.html

Contact
-------

Any remarks, patches, gift-cards and other related stuff can can go to:

 * aguinet@quarkslab.com

Authors
-------

See AUTHORS file.

License
-------

See LICENSE file.
