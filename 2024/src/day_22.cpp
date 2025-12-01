#include "utils.h"
#include <iostream>

namespace day_22 {
uint64_t mix(uint64_t secret, uint64_t val) { return secret ^ val; }

uint64_t prune(uint64_t secret) { return secret % 16777216ul; }

uint64_t get_secret(uint64_t initial) {
    initial = mix(initial, initial << 6);
    initial = prune(initial);
    initial = mix(initial, initial >> 5);
    initial = prune(initial);
    initial = mix(initial, initial * 2048);
    initial = prune(initial);

    return initial;
}

int hash_4(int vals[4]) {
    int hash = 0;

    for (int i = 0; i < 4; i++) {
        int part = abs(vals[i]) | ((vals[i] < 0) << 4);
        hash |= part << (i * 6);
    }

    return hash;
}

uint64_t run_pt1_(std::vector<std::string> &lines) {
    uint64_t total = 0;
    for (auto line : lines) {
        uint64_t initial_secret = std::stoull(line);
        uint64_t secret = initial_secret;
        for (int i = 0; i < 2000; i++) {
            secret = get_secret(secret);
        }

        total += secret;
    }

    return total;
}

uint64_t run(std::vector<std::string> &lines, bool is_part1) {
    if (is_part1) return run_pt1_(lines);

    auto monkey_trends = std::unordered_map<int, std::vector<int>>();

    for (int monkey = 0; monkey < lines.size(); monkey++) {
        uint64_t secret = std::stoull(lines[monkey]);
        int diffs[2000] = {0};
        int prev_price = secret % 10;

        for (int i = 0; i < 2000; i++) {
            secret = get_secret(secret);
            int price = secret % 10;
            diffs[i] = price - prev_price;
            prev_price = price;

            if (i < 3) continue;

            int segment[4] = {
                diffs[i - 3],
                diffs[i - 2],
                diffs[i - 1],
                diffs[i],
            };
            int hash = hash_4(segment);

            if (!monkey_trends.contains(hash)) {
                monkey_trends[hash] = std::vector<int>(lines.size(), 0);
            }

            std::vector<int> &trends = monkey_trends[hash];
            if (trends[monkey] == 0) trends[monkey] = price;
        }
    }

    int max = 0;
    for (auto [_, trend] : monkey_trends) {
        int sum = 0;
        for (int price : trend) {
            sum += price;
        }
        if (sum > max) max = sum;
    }

    return max;
}
} // namespace day_22
