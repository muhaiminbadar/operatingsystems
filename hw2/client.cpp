#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct CharData {
    char symbol;
    int frequency;
};

int main (int argc, char* argv[]) {
    std::vector<CharData> symbols;
    std::string line;
    std::getline(std::cin, line);
    int totalChars = line.size();
    for(char& c : line) {
        auto it = std::find_if(symbols.begin(), symbols.end(), [&c] (const CharData& e) {
            return e.symbol == c;
        });        
        if(it != symbols.end()) {
            it->frequency++;
        } else {
            symbols.push_back({c, 1});
        }
    }

    std::sort(symbols.begin(), symbols.end(), [] (const auto& a, const auto& b) { 
        return a.frequency > b.frequency; 
    });

    for(auto [s, f] : symbols) {
        std::cout << "Symbol: " << s << ", Freq: " <<  (float) f/totalChars << std::endl;
    }
    return EXIT_SUCCESS;
}