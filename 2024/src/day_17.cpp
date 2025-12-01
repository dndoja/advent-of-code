#include "utils.h"
#include <bitset>
#include <iostream>
#include <queue>
#include <set>
#include <string>

namespace day_17 {
enum Opcode {
    // The adv instruction (opcode 0) performs division. The numerator is the
    // value in the A register. The denominator is found by raising 2 to the
    // power of the instruction's combo operand. (So, an operand of 2 would
    // divide A by 4 (2^2); an operand of 5 would divide A by 2^B.) The result
    // of the division operation is truncated to an integer and then written to
    // the A register.
    adv = 0,

    // The bxl instruction (opcode 1) calculates the bitwise XOR of register B
    // and the instruction's literal operand, then stores the result in register
    // B.
    bxl = 1,

    // The bst instruction (opcode 2) calculates the value of its combo operand
    // modulo 8 (thereby keeping only its lowest 3 bits), then writes that value
    // to the B register.
    bst = 2,

    // The jnz instruction (opcode 3) does nothing if the A register is 0.
    // However, if the A register is not zero, it jumps by setting the
    // instruction pointer to the value of its literal operand; if this
    // instruction jumps, the instruction pointer is not increased by 2 after
    // this instruction.
    jnz = 3,

    // The bxc instruction (opcode 4) calculates the bitwise XOR of register B
    // and register C, then stores the result in register B. (For legacy
    // reasons, this instruction reads an operand but ignores it.)
    bxc = 4,

    // The out instruction (opcode 5) calculates the value of its combo operand
    // modulo 8, then outputs that value. (If a program outputs multiple values,
    // they are separated by commas.)
    out = 5,

    // The bdv instruction (opcode 6) works exactly like the adv instruction
    // except that the result is stored in the B register. (The numerator is
    // still read from the A register.)
    bdv = 6,

    // The cdv instruction (opcode 7) works exactly like the adv instruction
    // except that the result is stored in the C register. (The numerator is
    // still read from the A register.)
    cdv = 7,
};

struct VM {
    std::vector<int> &stack;
    uint64_t a, b, c = 0;
    int curr_op_idx = 0;
    std::vector<uint64_t> out;

    int run(bool jump = true, bool print = false) {
        int counter = 0;

        while (curr_op_idx < stack.size()) {
            Opcode opcode = (Opcode)stack[curr_op_idx];
            int literal_val = stack[curr_op_idx + 1];
            int combo_val = operand_val(literal_val);
            int next_idx = curr_op_idx + 2;

            switch (opcode) {
            case Opcode::adv:
                if (print) std::cout << "adv " << literal_val << std::endl;
                a = a >> combo_val;
                break;
            case Opcode::bxl:
                if (print) std::cout << "bxl " << literal_val << std::endl;
                b ^= literal_val;
                break;
            case Opcode::bst:
                if (print) std::cout << "bst " << literal_val << std::endl;
                b = combo_val & 0b111;
                break;
            case Opcode::jnz:
                if (print) std::cout << "jnz " << literal_val << std::endl;
                if (a == 0) break;
                next_idx = jump ? literal_val : stack.size();
                break;
            case Opcode::bxc:
                if (print) std::cout << "bxc" << std::endl;
                b ^= c;
                break;
            case Opcode::out:
                if (print) {
                    std::cout << "out " << (combo_val & 0b111) << std::endl;
                    std::cout << std::endl;
                    std::cout << a << " " << std::bitset<64>(a) << " " << b
                              << " " << c << std::endl;
                }
                out.push_back(combo_val & 0b111);
                break;
            case Opcode::bdv:
                if (print) {
                    std::cout << "bdv " << literal_val << " " << combo_val
                              << std::endl;
                }
                b = a >> combo_val;
                break;
            case Opcode::cdv:
                if (print) {
                    std::cout << "cdv " << literal_val << " " << combo_val
                              << std::endl;
                }
                c = a >> combo_val;
                break;
            }

            counter++;
            curr_op_idx = next_idx;
        }

        return counter;
    }

    int operand_val(int operand) {
        if (operand <= 3) return operand;
        if (operand == 4) return a;
        if (operand == 5) return b;
        if (operand == 6) return c;
        return -1;
    }

    std::string output() {
        std::string output = "";
        for (int i = 0; i < out.size(); i++) {
            output += std::to_string(out[i]);
            if (i < out.size() - 1) output += ",";
        }

        return output;
    }
};

std::string run(std::vector<std::string> &lines, bool is_part1) {
    uint64_t initial_a = std::stoul(utils::split(lines[0], ": ")[1]);
    uint64_t initial_b = std::stoul(utils::split(lines[1], ": ")[1]);
    uint64_t initial_c = std::stoul(utils::split(lines[2], ": ")[1]);
    std::string program_str = utils::split(lines[4], ": ")[1];
    auto instructions_str = utils::split(program_str, ",");
    auto instructions = std::vector<int>();
    for (int i = 0; i < instructions_str.size(); i++) {
        int val = std::stoi(instructions_str[i]);
        instructions.push_back(val);
    }

    if (is_part1) {
        VM vm = {instructions, initial_a, initial_b, initial_c};
        vm.run(true, true);
        return vm.output();
    }

    auto valid_a_regs = std::vector<std::set<int>>(instructions.size());
    for (int i = 0; i < instructions.size(); i++) {
        for (uint64_t a = 0; a < 0b1111111111; a++) {
            VM vm = {instructions, a};
            vm.run(false);
            if (vm.out[0] == instructions[i]) valid_a_regs[i].insert(a);
        }
    }

    struct Node {
        int i, a;
        uint64_t a_full;
    };

    auto queue = std::queue<Node>();
    for (int a : valid_a_regs[0]) {
        Node node = {0, a, (uint64_t)a & 0b111};
        queue.push(node);
    }

    uint64_t result = -1;
    while (!queue.empty()) {
        Node curr = queue.front();
        queue.pop();

        VM vm = VM{instructions, (uint64_t)curr.a};
        vm.run(false, false);

        if (curr.i == instructions.size() - 1) {
            if (vm.a == 0 && (result == -1 || curr.a_full < result)) {
                result = curr.a_full;
            }

            continue;
        }

        int next_i = curr.i + 1;
        for (int next_a : valid_a_regs[next_i]) {
            if ((next_a & 0b0001111111) != vm.a) continue;

            queue.push({
                next_i,
                next_a,
                curr.a_full | (((uint64_t)next_a & 0b111) << next_i * 3),
            });
        }
    }

    return std::to_string(result);
}
}; // namespace day_17
   //
