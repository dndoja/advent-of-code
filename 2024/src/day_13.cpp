#include "utils.h"

namespace day_13 {
struct Offset {
    uint64_t dx;
    uint64_t dy;
};

uint64_t run(std::vector<std::string> const &lines, bool is_part1) {
    auto input = std::vector<std::tuple<Offset, Offset, Offset>>();

    for (int i = 0; i < lines.size(); i += 4) {
        std::string a_str = utils::split(lines[i], ": ")[1];
        std::string b_str = utils::split(lines[i + 1], ": ")[1];
        std::string prize_str = utils::split(lines[i + 2], ": ")[1];

        auto a_split = utils::split(a_str, ", ");
        Offset a = {
            std::stoull(a_split[0].substr(2)),
            std::stoull(a_split[1].substr(2)),
        };

        auto b_split = utils::split(b_str, ", ");
        Offset b = {
            std::stoull(b_split[0].substr(2)),
            std::stoull(b_split[1].substr(2)),
        };

        auto prize_split = utils::split(prize_str, ", ");
        uint64_t prize_x = std::stoll(prize_split[0].substr(2));
        uint64_t prize_y = std::stoi(prize_split[1].substr(2));

        if (!is_part1) {
            prize_x += 10000000000000;
            prize_y += 10000000000000;
        }

        input.push_back({a, b, {prize_x, prize_y}});
    }

    uint64_t min_tokens = 0;

    for (auto [a, b, prize] : input) {
        /// y = mx + b (m = Slope, b = Y-Intercept)
        double a_slope = (double)a.dy / a.dx;
        double b_slope = (double)b.dy / b.dx;

        /// Shift "a" line to include prize
        double a_line_shifted_y_intercept =
            (double)prize.dy - (prize.dx * a_slope);

        uint64_t intersect_x =
            round(-a_line_shifted_y_intercept / (a_slope - b_slope));
        uint64_t intersect_y = round(intersect_x * b_slope);
        uint64_t mod_a = ((prize.dx - intersect_x) % a.dx) +
                         ((prize.dy - intersect_y) % a.dy);
        uint64_t mod_b = intersect_x % b.dx + intersect_y % b.dy;

        if (mod_a == 0 && mod_b == 0) {
            uint64_t dist_a = (prize.dx - intersect_x) / a.dx;
            uint64_t dist_b = intersect_x / b.dx;
            min_tokens += dist_a * 3 + dist_b;
        }
    }

    return min_tokens;
} // namespace day_13
} // namespace day_13
