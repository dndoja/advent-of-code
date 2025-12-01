#include "utils.h"
#include <iostream>
#include <unordered_set>

namespace day_23 {
int run_pt1(std::unordered_map<int, std::unordered_set<int>> &routes) {
    int count = 0;
    std::unordered_set<int> trios = {};

    for (auto [conn1, connections] : routes) {
        for (int conn2 : connections) {
            for (int conn3 : routes[conn2]) {
                if (conn3 == conn1 || !connections.contains(conn3)) continue;

                int trio[3] = {conn1, conn2, conn3};
                int n = sizeof(trio) / sizeof(trio[0]);
                std::sort(trio, trio + n);

                int hash = trio[0] | trio[1] << 10 | trio[2] << 20;

                if (trios.contains(hash)) continue;
                trios.insert(hash);

                if ((trio[0] >> 5) + 'a' == 't' ||
                    (trio[1] >> 5) + 'a' == 't' ||
                    (trio[2] >> 5) + 'a' == 't') {
                    count++;
                }
            }
        }
    }

    return count;
}

std::string run(std::vector<std::string> &lines, bool is_part1) {
    std::unordered_map<int, std::unordered_set<int>> routes = {};

    for (auto line : lines) {
        std::vector<std::string> split = utils::split(line, "-");
        std::string l = split[0];
        std::string r = split[1];
        int l_id = (l[0] - 'a') << 5 | (l[1] - 'a');
        int r_id = (r[0] - 'a') << 5 | (r[1] - 'a');

        routes[l_id].insert(r_id);
        routes[r_id].insert(l_id);
    }

    if (is_part1) return std::to_string(run_pt1(routes));

    std::vector<std::unordered_set<int>> clusters = {};
    for (auto [route, _] : routes) {
        clusters.push_back({route});
    }
    std::unordered_set<int> biggest = clusters[0];

    for (auto [route, connected] : routes) {
        for (std::unordered_set<int> &cluster : clusters) {
            bool valid = true;
            for (int other : cluster) {
                if (!connected.contains(other) ||
                    !routes[other].contains(route)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                cluster.insert(route);
                if (cluster.size() > biggest.size()) biggest = cluster;
            }
        }
    }

    auto biggest_sorted = std::vector<int>(biggest.begin(), biggest.end());
    std::sort(biggest_sorted.begin(), biggest_sorted.end());

    std::string result = "";
    for (int i = 0; i < biggest.size(); i++) {
        result += 'a' + (biggest_sorted[i] >> 5);
        result += 'a' + (biggest_sorted[i] & 0b11111);
        if (i < biggest_sorted.size() - 1) result += ',';
    }

    return result;
}
} // namespace day_23
  //
