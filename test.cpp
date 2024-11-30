#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip> // for std::setprecision

struct Trade {
    long long timeStamp;
    std::string symbol;
    int quantity;
    int price;
};

struct SymbolStats {
    long long maxTimeGap = 0;
    int totalVolume = 0;
    int weightedAveragePrice = 0;
    int maxPrice = 0;
    long long lastTimeStamp = 0;

    void addTrade(const Trade& trade) {
        if (lastTimeStamp != 0) {
            maxTimeGap = std::max(maxTimeGap, trade.timeStamp - lastTimeStamp);
        }
        totalVolume += trade.quantity;
        weightedAveragePrice = ((weightedAveragePrice * totalVolume) + (trade.quantity * trade.price)) / (totalVolume + 1); // update weighted average
        maxPrice = std::max(maxPrice, trade.price);
        lastTimeStamp = trade.timeStamp;

        // Since we are using integer division for weighted average, truncate the float result
        weightedAveragePrice = static_cast<int>(weightedAveragePrice);
    }
};

int main() {
    std::ifstream inputFile("input.csv");
    std::ofstream outputFile("output_.csv");

    if (!inputFile.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return 1;
    }

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return 1;
    }

    std::string line;
    std::map<std::string, SymbolStats> symbolStats;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string timeStampStr, symbol, quantityStr, priceStr;
        if (std::getline(iss, timeStampStr, ',') &&
            std::getline(iss, symbol, ',') &&
            std::getline(iss, quantityStr, ',') &&
            std::getline(iss, priceStr, ',')) {

            Trade trade;
            trade.timeStamp = std::stoll(timeStampStr);
            trade.symbol = symbol;
            trade.quantity = std::stoi(quantityStr);
            trade.price = std::stoi(priceStr);

            symbolStats[trade.symbol].addTrade(trade);
        }
    }

    inputFile.close();

    // Prepare output
    std::vector<std::pair<std::string, SymbolStats>> sortedSymbolStats(symbolStats.begin(), symbolStats.end());
    std::sort(sortedSymbolStats.begin(), sortedSymbolStats.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for (const auto& [symbol, stats] : sortedSymbolStats) {
        outputFile << symbol << ","
                   << stats.maxTimeGap << ","
                   << stats.totalVolume << ","
                   << stats.weightedAveragePrice << ","
                   << stats.maxPrice << std::endl;
    }

    outputFile.close();
    return 0;
}
// using about 30min to finish this test.
