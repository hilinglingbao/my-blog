#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <numeric>

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
    std::vector<long long> timeStamps;

    // Helper function to calculate weighted average price
    void calculateWeightedAveragePrice() {
        int totalWeightedPrice = 0;
        for (const auto& timeStamp : timeStamps) {
            // Find the trade corresponding to this timeStamp
            auto it = std::find_if(trades.begin(), trades.end(), [&](const Trade& t) {
                return t.timeStamp == timeStamp;
            });
            if (it != trades.end()) {
                const Trade& trade = *it;
                totalWeightedPrice += trade.quantity * trade.price;
            }
        }
        weightedAveragePrice = totalVolume > 0 ? totalWeightedPrice / totalVolume : 0;
    }

    // Static container to hold all trades for easy access during calculations
    static std::vector<Trade> trades;
};

std::vector<Trade> SymbolStats::trades;

int main() {
    std::ifstream inFile("input.csv");
    std::ofstream outFile("output.csv");

    if (!inFile.is_open()) {
        std::cerr << "Error opening input file!" << std::endl;
        return 1;
    }

    std::string line;
    std::map<std::string, SymbolStats> statsMap;

    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string timeStampStr, symbol, quantityStr, priceStr;
        std::getline(ss, timeStampStr, ',');
        std::getline(ss, symbol, ',');
        std::getline(ss, quantityStr, ',');
        std::getline(ss, priceStr, ',');

        long long timeStamp = std::stoll(timeStampStr);
        int quantity = std::stoi(quantityStr);
        int price = std::stoi(priceStr);

        Trade trade{timeStamp, symbol, quantity, price};
        SymbolStats::trades.push_back(trade);

        statsMap[symbol].timeStamps.push_back(timeStamp);
        statsMap[symbol].totalVolume += quantity;
        statsMap[symbol].maxPrice = std::max(statsMap[symbol].maxPrice, price);
    }

    inFile.close();

    // Sort time stamps for each symbol to calculate max time gap
    for (auto& [symbol, stats] : statsMap) {
        std::sort(stats.timeStamps.begin(), stats.timeStamps.end());
        for (size_t i = 1; i < stats.timeStamps.size(); ++i) {
            stats.maxTimeGap = std::max(stats.maxTimeGap, stats.timeStamps[i] - stats.timeStamps[i - 1]);
        }
        stats.calculateWeightedAveragePrice(); // Calculate weighted average price
    }

    // Prepare output
    for (const auto& [symbol, stats] : statsMap) {
        outFile << symbol << ","
                << stats.maxTimeGap << ","
                << stats.totalVolume << ","
                << stats.weightedAveragePrice << ","
                << stats.maxPrice << std::endl;
    }

    outFile.close();

    // Clear static trades vector to avoid memory leak (optional, depending on program lifecycle)
    SymbolStats::trades.clear();

    return 0;
}