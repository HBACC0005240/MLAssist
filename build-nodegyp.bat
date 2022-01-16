cd CGANode

call node-gyp configure build --arch=ia32

mkdir "..\bin\Release\"

copy "build\Release\node_cga.node" "..\bin\Release\"