import json
import sys
from argparse import ArgumentParser

FILE_HEADER = """\
#include "cpu.h"

void Cpu::initOps(void) {
"""



FILE_FOOTER = """\
}
"""

OP_FMT     = "{}mOpTable[0x{:02x}].init({});\n"
EXT_OP_FMT = "{}mExtOpTable[0x{:02x}].init({});\n"
HANDLER_FMT = "&Cpu::oph_{}"

KEY_OPS_BASE = "base"
KEY_OPS_EXT = "ext"

KEY_OP_P1 = "p1"
KEY_OP_P2 = "p2"
KEY_OP_CYCLES = "cycles"
KEY_OP_BRANCH_CYCLES = "branch_cycles"
KEY_OP_MNEMONIC = "mnemonic"
KEY_OP_ADDR = "addr"

KEY_HANDLER_PARAMS = "params"
KEY_HANDLER_NAME   = "name"
KEY_HANDLER_CAN_BRANCH = "can_branch"
KEY_HANDLER_OPS = "ops"
KEY_HANDLER_EXT_OPS = "ext_ops"

class OptableGenerator(object):
    def __init__(self, handlers, opcodes, indent=4):
        self._handlers = handlers
        self._base_ops = opcodes[KEY_OPS_BASE]
        self._ext_ops = opcodes[KEY_OPS_EXT]
        self._indent = " " * indent

    def _process_op(self, output, handler, op, fmt):
        name = handler[KEY_HANDLER_NAME]
        num_param = handler[KEY_HANDLER_PARAMS]
        can_branch = handler.get(KEY_HANDLER_CAN_BRANCH, False)

        params = [ HANDLER_FMT.format(name), str(op[KEY_OP_CYCLES]) ]

        if can_branch:
            params.append(str(op[KEY_OP_BRANCH_CYCLES]))

        params.append("\"{}\"".format(op[KEY_OP_MNEMONIC]))

        if num_param > 0:
            params.append(str(op[KEY_OP_P1]))

            if num_param > 1:
                params.append(str(op[KEY_OP_P2]))

        pstr = ", ".join(params)

        output.write(fmt.format(self._indent, op[KEY_OP_ADDR], ", ".join(params)))


    def _process_handler(self, output, handler):
        output.write("\n{}// {}\n".format(self._indent, handler[KEY_HANDLER_NAME]))
        for op in handler[KEY_HANDLER_OPS]:
            self._process_op(output, handler, self._base_ops[op], OP_FMT)

        if KEY_HANDLER_EXT_OPS in handler:
            for op in handler[KEY_HANDLER_EXT_OPS]:
                self._process_op(output, handler, self._ext_ops[op], EXT_OP_FMT)

    def write_file(self, output):
        output.write(FILE_HEADER)
        for h in self._handlers:
            self._process_handler(output, h)
        output.write(FILE_FOOTER)




if __name__ == "__main__":
    parser = ArgumentParser("Generate GB OP Table based on json configuration")
    parser.add_argument("handlers_file")
    parser.add_argument("opcodes_file")

    args = parser.parse_args()

    with open(args.handlers_file, "r") as hfile, open(args.opcodes_file, "r") as ofile, open("table.c", "w") as outfile:
        handlers = json.load(hfile)
        opcodes = json.load(ofile)

        gen = OptableGenerator(handlers, opcodes)
        gen.write_file(sys.stdout)

