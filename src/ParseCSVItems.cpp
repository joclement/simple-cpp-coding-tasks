/**
 * Interview-like programm task. Task is:
 * 1. Read a CSV file containing information about items. Each item has the
 *    following information:
 *     - the name
 *     - the quantity
 *     - either the price for one item or the price for all items (quantity)
 * 2. Construct suitable item objects according to the information given in
 *    the CSV file
 * 3. Add price count of items with same name
 * 4. Print items to stdout
 *
 * Contstraints:
 * - Do not use any libraries except STL
 */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

std::ifstream open(std::filesystem::path path) {
    // Open the stream to 'lock' the file.
    std::ifstream f(path, std::ios::in | std::ios::binary);
    return f;
}

struct Item {
    Item(std::list<std::string> &&items) {
        name = items.front();
        items.pop_front();

        count = std::stoi(items.front());
        items.pop_front();

        std::string singlePriceStr = items.front();
        if (singlePriceStr.empty()) {
            items.pop_front();
            totalPrice = std::stof(items.front());
        } else {
            totalPrice = std::stof(singlePriceStr) * count;
        }

        min = singlePrice();
        max = singlePrice();
    }

    float singlePrice() const { return totalPrice / count; }

    Item &operator+=(const Item &other) {
        if (name != other.name) {
            throw std::invalid_argument("Names unequal");
        }
        count += other.count;
        totalPrice += other.totalPrice;

        min = std::min({min, singlePrice(), other.singlePrice()});
        max = std::max({max, singlePrice(), other.singlePrice()});

        return *this;
    }

    std::string name;
    unsigned count;
    float totalPrice;
    float min;
    float max;
};

std::list<std::string> split(std::istream &input, char delimiter) {
    std::list<std::string> lines;
    {
        std::string line;
        while (std::getline(input, line, delimiter)) {
            lines.push_back(line);
        }
    }

    return lines;
}

std::list<std::string> splitLines(std::istream &input) {
    return split(input, '\n');
}

std::list<std::string> readItemLines(const std::string &filepath) {
    auto input = open(filepath);
    auto lines = splitLines(input);
    lines.pop_front();

    return lines;
}

std::list<Item> construct(std::list<std::string> &&lines) {
    std::list<Item> items;

    for (const auto &line : lines) {
        std::istringstream input;
        input.str(line);

        items.push_back(Item(split(input, ';')));
    }

    return items;
}

std::map<std::string, Item> accumulate(std::list<Item> &&items) {
    std::map<std::string, Item> accItems;

    for (const auto &item : items) {
        if (accItems.count(item.name) == 0) {
            accItems.insert({item.name, item});
        } else {
            accItems.at(item.name) += item;
        }
    }

    return accItems;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return -1;
    }
    std::string filepath(argv[1]);

    auto items = accumulate(construct(readItemLines(filepath)));

    for (const auto &mapItem : items) {
        auto item = mapItem.second;
        std::cout << item.name << ", " << item.count << ", "
                  << item.singlePrice() << ", " << item.min << ", " << item.max
                  << std::endl;
    }

    return EXIT_SUCCESS;
}
