## Boldboy Tools

### opcodes.json
A breakdown of information for each Game CPU (Sharp LR35902) opcode. This json file comes from https://github.com/lmmendes/game-boy-opcodes, which in turn scraped the data from http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html.

### parseops.py
A simple python script to scrape relevant information our of the opcodes.json file and generate the opcode initialization table used in cpu_opcodes.cpp. It currently assigns the NOP handler to all opcodes, but fills in cycle and mnemonic information.
