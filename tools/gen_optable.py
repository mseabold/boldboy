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
        self._ops_used = [False]*256
        self._ext_ops_used = [False]*256

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
            if self._ops_used[op]:
                raise ValueError("Opcode 0x{:02x} is duplicated".format(op))

            self._ops_used[op] = True
            self._process_op(output, handler, self._base_ops[op], OP_FMT)

        if KEY_HANDLER_EXT_OPS in handler:
            for op in handler[KEY_HANDLER_EXT_OPS]:
                if self._ext_ops_used[op]:
                    raise ValueError("Opcode 0x{:02x} is duplicated".format(op))

                self._ext_ops_used[op] = True
                self._process_op(output, handler, self._ext_ops[op], EXT_OP_FMT)

    def write_file(self, output):
        output.write(FILE_HEADER)
        for h in self._handlers:
            self._process_handler(output, h)
        output.write(FILE_FOOTER)

    def gen_report(self):
        unused = 256
        used = 0
        for h in self._handlers:
            for op in h[KEY_HANDLER_OPS]:
                if self._ops_used[op]:
                    raise ValueError("Opcode 0x{:02x} is duplicated".format(op))

                unused -= 1
                used += 1

                self._ops_used[op] = True

        print("{} opcodes used. {} opcodes unused".format(used, unused))

        for i in range(256):
            if not self._ops_used[i]:
                print("Unused op 0x{:02x}: {}".format(i, self._base_ops[i][KEY_OP_MNEMONIC]))




if __name__ == "__main__":
    parser = ArgumentParser("Generate GB OP Table based on json configuration")
    parser.add_argument("handlers_file")
    parser.add_argument("opcodes_file")
    parser.add_argument("-f", "--output-file", help="Output C++ table file")
    parser.add_argument("-r", "--report", help="Generate a report on unused opcodes", action="store_true", default=False)

    args = parser.parse_args()

    if args.output_file:
        outfile = open(args.output_file, "w")
    else:
        outfile = sys.stdout

    with open(args.handlers_file, "r") as hfile, open(args.opcodes_file, "r") as ofile:
        handlers = json.load(hfile)
        opcodes = json.load(ofile)

        gen = OptableGenerator(handlers, opcodes)

        if args.report:
            gen.gen_report()
        else:
            gen.write_file(outfile)

    if args.output_file:
        outfile.close()

