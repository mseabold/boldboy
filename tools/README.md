## Boldboy Tools

### opcodes.json
A breakdown of information for each Game CPU (Sharp LR35902) opcode. This json file had its origins from https://github.com/lmmendes/game-boy-opcodes, which in turn scraped the data from http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html, however it has since been heavily modified into a format suitable for the autogenerator.

### handlers.json
A list of all the available opcode handlers currently implemented in the Boldboy CPU. This lists the expeted number of parameters, whether it can cause a branch, the name of the handler function in C++, and the list of opcode and extended opcode indices that will utilize the handler.

### gen_optable.py
Python script which generates a C++ file (as part of the Cpu class) which initialized the CPU opcode tables. It walks the list of all handlers in the handlers json file and pulls information such as parameters, cycles, and mnemonics from the opcodes json for each opcode the handler lists. CMake includes this script as part of the build dependency tree in order to create the optable, and will cause an update whenever this script or either json file changes.
