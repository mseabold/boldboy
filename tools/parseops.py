import json
import sys
import argparse

def print_ops(ops, table_name):
    for i in range(256):
        i_str = "0x{:x}".format(i)

        if i_str in ops:
            op = ops[i_str]
            mnemonic = op["mnemonic"]

            if "operand1" in op:
                mnemonic += " {}".format(op["operand1"])

            if "operand2" in op:
                mnemonic += ",{}".format(op["operand2"])

            cycles = op["cycles"]

            if len(cycles) == 1:
                print("    {}[0x{:02x}].init(&Cpu::oph_Nop, {}, \"{}\");".format(table_name, i, cycles[0], mnemonic))
            else:
                print("    {}[0x{:02x}].init(&Cpu::oph_Nop, {}, {}, \"{}\");".format(table_name, i, cycles[1], cycles[0], mnemonic))
        else:
            print("    {}[0x{:02x}].init(&Cpu::oph_Invalid, 0, \"Invalid\");".format(table_name, i))

if __name__ == "__main__":
    parser = argparse.ArgumentParser("parseops", "Parse Gameboy opcodes json until C initializers")
    parser.add_argument("json_file")

    args = parser.parse_args()

    with open(args.json_file, "r") as f:
        ops = json.load(f)

        unprefixed = ops["unprefixed"]
        cbprefixed = ops["cbprefixed"]

        print_ops(unprefixed, "mOpTable")
        print()
        print_ops(cbprefixed, "mCbOpTable")



