#include "utils.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_set>

namespace day_24 {

int id(std::string const &str) {
    return 0 | str[0] | ((int)str[2] << 8) | ((int)str[1] << 16);
}

std::string id_str(int hash) {
    std::string result = "";
    result += hash & 0b11111111;
    result += (hash >> 16) & 0b11111111;
    result += (hash >> 8) & 0b11111111;
    return result;
}

enum Op { OR, XOR, AND };

struct Gate {
    int l, r, out;
    Op op;

    int id() { return op | l << 3 | r << 27; }

    std::string to_string() {
        std::string str = id_str(l);

        switch (op) {
        case OR:
            str += " OR ";
            break;
        case XOR:
            str += " XOR ";
            break;
        case AND:
            str += " AND ";
            break;
        };
        str += id_str(r) + " -> " + id_str(out);

        return str;
    }
};

/*

Full Adder

A-----|   |
  |   |XOR|--
B-|---|   | |   |   |
  | |       |---|XOR|----------SUM
  | |       |C--|   |
  | |       ||
  | |       ||             |   |CNXB
  | |       |--------------|AND|-----|  |
  | |       |--------------|   |     |  |
  | |                                |OR|---- C-out
  |_|______________________|   |ANB  |  |
    |                      |AND|-----|  |
    |______________________|   |


A ^ B ^ C = SUM
(A & B) | (A ^ B & C) = C-OUT

*/

enum Label {
    A,
    B,
    CIN,
    COUT,
    AXB,
    ANB,
    CNXB,
    SUM,
};

struct Step {
    Label l;
    Op op;
    Label r;
    Label dest;
};

std::string run_pt1(std::vector<Gate *> &initial_gates,
                    std::unordered_map<int, bool> &wires,
                    std::unordered_map<int, std::vector<Gate *>> dependencies) {
    struct WireValue {
        int id;
        bool value;
    };

    for (int i = 0; i < initial_gates.size(); i += 2) {
        std::queue<Gate *> queue = {};
        queue.push(initial_gates[i]);
        queue.push(initial_gates[i + 1]);

        while (!queue.empty()) {
            Gate *gate_ptr = queue.front();
            Gate gate = *gate_ptr;
            queue.pop();

            bool value;
            switch (gate.op) {
            case OR:
                value = wires[gate.l] || wires[gate.r];
                break;
            case XOR:
                value = wires[gate.l] ^ wires[gate.r];
                break;
            case AND:
                value = wires[gate.l] && wires[gate.r];
                break;
            }

            wires[gate.out] = value;

            for (Gate *dep : dependencies[gate.out]) {
                if (wires.contains(dep->l) && wires.contains(dep->r)) {
                    queue.push(dep);
                }
            }
        }
    }

    std::vector<WireValue> values = {};
    for (auto [wire_id, value] : wires) {
        if ((wire_id & 0b11111111) != 'z') continue;
        values.push_back({wire_id, value});
    }

    std::sort(
        values.begin(), values.end(),
        [](WireValue const &a, WireValue const &b) { return a.id < b.id; });

    uint64_t result = 0;
    for (int i = 0; i < values.size(); i++) {
        result |= (uint64_t)values[i].value << i;
    }

    return std::to_string(result);
}

std::string run(std::vector<std::string> &lines, bool is_part1) {
    std::unordered_map<int, bool> wires = {};
    std::vector<Gate> gates = std::vector<Gate>(lines.size());
    std::unordered_map<int, std::vector<Gate *>> dependencies = {};
    std::vector<Gate *> initial_gates = {};

    for (auto line : lines) {
        if (line.empty()) continue;

        auto split = utils::split(line, ": ");

        if (split.size() == 2) {
            wires[id(split[0])] = std::stoi(split[1]);
        } else {
            gates.push_back({});
            Gate &gate = gates.back();

            split = utils::split(line, " -> ");
            gate.out = id(split[1]);

            split = utils::split(split[0], " ");
            if (split[0] < split[2]) {
                gate.l = id(split[0]);
                gate.r = id(split[2]);
            } else {
                gate.l = id(split[2]);
                gate.r = id(split[0]);
            }

            if (split[1] == "XOR") {
                gate.op = Op::XOR;
            } else if (split[1] == "OR") {
                gate.op = Op::OR;
            } else if (split[1] == "AND") {
                gate.op = Op::AND;
            }

            dependencies[gate.l].push_back(&gate);
            dependencies[gate.r].push_back(&gate);
            if (wires.contains(gate.l) && wires.contains(gate.r)) {
                initial_gates.push_back(&(gates[gates.size() - 1]));
            }
        }
    }

    std::sort(initial_gates.begin(), initial_gates.end(),
              [](Gate *a, Gate *b) { return a->id() < b->id(); });

    if (is_part1) return run_pt1(initial_gates, wires, dependencies);

    // clang-format off
    constexpr Step full_adder_steps[] = {
        {A, XOR, B, AXB},
        {A, AND, B, ANB},
        {CIN, AND, AXB, CNXB},
        {CIN, XOR, AXB, SUM},
        {CNXB, OR, ANB, COUT},
    };
    // clang-format on

    struct Output {
        Gate *src;
        int id;
    };

    Output prev_cout = {};
    std::unordered_set<Gate *> wrong_gates = {};
    std::unordered_map<int, int> wrong_outs = {};

    for (int i = 0; i < initial_gates.size(); i += 2) {
        std::queue<Gate *> queue = {};
        queue.push(initial_gates[i]);
        queue.push(initial_gates[i + 1]);

        std::unordered_map<Label, Output> outputs = {};
        outputs[A] = {initial_gates[i], initial_gates[i]->l};
        outputs[B] = {initial_gates[i], initial_gates[i]->r};
        outputs[AXB] = {initial_gates[i], initial_gates[i]->out};
        outputs[ANB] = {initial_gates[i + 1], initial_gates[i + 1]->out};
        outputs[CIN] = prev_cout;

        auto print_outputs = [&outputs] {
            if (outputs.contains(A))
                std::cout << "A: " << id_str(outputs[A].id) << std::endl;
            if (outputs.contains(B))
                std::cout << "B: " << id_str(outputs[B].id) << std::endl;
            if (outputs.contains(AXB))
                std::cout << "AXB: " << id_str(outputs[AXB].id) << std::endl;
            if (outputs.contains(ANB))
                std::cout << "ANB: " << id_str(outputs[ANB].id) << std::endl;
            if (outputs.contains(CNXB))
                std::cout << "CNXB: " << id_str(outputs[CNXB].id) << std::endl;
            if (outputs.contains(CIN))
                std::cout << "CIN: " << id_str(outputs[CIN].id) << std::endl;
            if (outputs.contains(COUT))
                std::cout << "COUT: " << id_str(outputs[COUT].id) << std::endl;

            std::cout << std::endl;
        };

        int curr_op = 0;

        while (!queue.empty()) {
            Gate *gate_ptr = queue.front();
            Gate gate = *gate_ptr;
            queue.pop();

            Step step;
            switch (gate.op) {
            case OR:
                step = full_adder_steps[4];
                break;
            case XOR:
                step = curr_op == 0 ? full_adder_steps[0] : full_adder_steps[3];
                break;
            case AND:
                step = curr_op == 1 ? full_adder_steps[1] : full_adder_steps[2];
                break;
            }

            if (i > 2) {
                int actual_1 = outputs[step.l].id;
                int actual_2 = outputs[step.r].id;

                if (gate.l == actual_1) {
                    if (gate.r != actual_2) wrong_outs[actual_2] = gate.r;
                } else if (gate.l == actual_2) {
                    if (gate.r != actual_1) wrong_outs[actual_1] = gate.r;
                } else if (gate.r == actual_1) {
                    if (gate.l != actual_2) wrong_outs[actual_2] = gate.l;
                } else if (gate.r == actual_2) {
                    if (gate.l != actual_1) wrong_outs[actual_1] = gate.l;
                }
            }

            outputs[step.dest] = {gate_ptr, gate.out};
            if (step.dest == COUT) prev_cout = {gate_ptr, gate.out};

            bool value;
            switch (gate.op) {
            case OR:
                value = wires[gate.l] || wires[gate.r];
                break;
            case XOR:
                value = wires[gate.l] ^ wires[gate.r];
                break;
            case AND:
                value = wires[gate.l] && wires[gate.r];
                break;
            }

            wires[gate.out] = value;

            for (Gate *dep : dependencies[gate.out]) {
                if (wires.contains(dep->l) && wires.contains(dep->r)) {
                    queue.push(dep);
                }
            }

            curr_op++;
        }
    }

    auto swapped = std::unordered_set<int>();
    auto swaps = std::vector<std::string>();
    for (auto [wrong, right] : wrong_outs) {
        if (swapped.contains(wrong ^ right)) continue;
        swapped.insert(wrong ^ right);

        swaps.push_back(id_str(wrong));
        swaps.push_back(id_str(right));
    }

    std::sort(swaps.begin(), swaps.end());

    std::string result = "";
    for (int i = 0; i < swaps.size(); i++) {
        result += swaps[i];
        if (i < swaps.size() - 1) result += ",";
    }

    return result;
}
} // namespace day_24
