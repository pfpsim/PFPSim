Debugger Dependencies                 {#debug_depends}
=================================
Protocol Buffers v2.6.1: https://github.com/google/protobuf/releases/tag/v2.6.1
    - Installation: follow protobuf's README.md file with the following modifications
          - use the following configure command:
                ./configure CC=clang CXX=clang++ CXXFLAGS='-std=c++11 -stdlib=libstdc++ -O3 -g' LDFLAGS='-stdlib=libstdc++' LIBS="-lc++ -lc++abi"
                make
                sudo make install
                sudo ldconfig # may or may not be necessary
                if using python2.7: sudo pip2 install protobuf
                if using python3: sudo pip3 install protobuf==3.0.0b2
          - 'make check' will fail and so don't bother running it.

nanomsg v0.5-beta: http://download.nanomsg.org/nanomsg-0.5-beta.tar.gz

nnpy (nanomsg python package): https://github.com/nanomsg/nnpy
    - Installation:
          - if using python2.7: sudo pip2 install nnpy
          - if using python3: sudo pip3 install nnpy

    -- Note: nanomsg and nnpy are also used by P4. You may also use P4's install scripts in behavioral-model/build/travis

Python tabulate library: https://pypi.python.org/pypi/tabulate
    - Installation:
          - if using python2.7: sudo pip2 install tabulate
          - if using python3: sudo pip3 install tabulate
