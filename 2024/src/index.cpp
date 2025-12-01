// clang-format off
#include "day_1.cpp"
#include "day_2.cpp"
#include "day_3.cpp"
#include "day_4.cpp"
#include "day_5.cpp"
#include "day_6.cpp"
#include "day_7.cpp"
#include "day_8.cpp"
#include "day_9.cpp"
#include "day_10.cpp"
#include "day_11.cpp"
#include "day_12.cpp"
#include "day_13.cpp"
#include "day_14.cpp"
#include "day_15.cpp"
#include "day_16.cpp"
#include "day_17.cpp"
#include "day_18.cpp"
#include "day_19.cpp"
#include "day_20.cpp"
#include "day_21.cpp"
#include "day_22.cpp"
#include "day_23.cpp"
#include "day_24.cpp"
#include "day_25.cpp"

std::string run_day(int day_nr, std::vector<std::string> &lines, bool isPart1) {
    switch (day_nr) {
    case  1: return std::to_string(day_1::run(lines, isPart1));
    case  2: return std::to_string(day_2::run(lines, isPart1));
    case  3: return std::to_string(day_3::run(lines, isPart1));
    case  4: return std::to_string(day_4::run(lines, isPart1));
    case  5: return std::to_string(day_5::run(lines, isPart1));
    case  6: return std::to_string(day_6::run(lines, isPart1));
    case  7: return std::to_string(day_7::run(lines, isPart1));
    case  8: return std::to_string(day_8::run(lines, isPart1));
    case  9: return std::to_string(day_9::run(lines, isPart1));
    case 10: return std::to_string(day_10::run(lines, isPart1));
    case 11: return std::to_string(day_11::run(lines, isPart1));
    case 12: return std::to_string(day_12::run(lines, isPart1));
    case 13: return std::to_string(day_13::run(lines, isPart1));
    case 14: return std::to_string(day_14::run(lines, isPart1));
    case 15: return std::to_string(day_15::run(lines, isPart1));
    case 16: return std::to_string(day_16::run(lines, isPart1));
    case 17: return day_17::run(lines, isPart1);
    case 18: return day_18::run(lines, isPart1);
    case 19: return std::to_string(day_19::run(lines,isPart1));
    case 20: return std::to_string(day_20::run(lines,isPart1));
    case 21: return std::to_string(day_21::run(lines,isPart1));
    case 22: return std::to_string(day_22::run(lines,isPart1));
    case 23: return day_23::run(lines,isPart1);
    case 24: return day_24::run(lines,isPart1);
    case 25: return std::to_string(day_25::run(lines,isPart1));
    default:
        return "error: not implemented yet!\n";
    }
}
