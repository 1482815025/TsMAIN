#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

struct Config {
    std::string dbc_file;
    std::string ldf_file;
    std::string dbc_node;
    bool lin_master;
    int routinTableId;
    std::string ldf_node;
    std::vector<std::tuple<unsigned long, std::string, double>> can_signals;
    std::vector<std::tuple<unsigned long, std::string, double>> lin_signals;
    Config() : dbc_file(""), ldf_file(""), dbc_node(""), lin_master(false), routinTableId(-1), ldf_node(""), can_signals(), lin_signals() {}
};

void parseSignals(const std::string& valuesStr, std::vector<std::tuple<unsigned long, std::string, double>>& signals) {
    std::regex signalRegex(R"(<(0x[0-9A-Fa-f]+),([^,]+),(\d+)>)");
    std::sregex_iterator begin(valuesStr.begin(), valuesStr.end(), signalRegex);
    std::sregex_iterator end;

    while (begin != end) {
        std::smatch match = *begin;
        unsigned long id = std::stoul(match[1].str(), nullptr, 16);
        std::string signalName = match[2].str();
        double value = std::stod(match[3].str());
        signals.emplace_back(id, signalName, value);
        begin++;
    }
}

std::string trim(const std::string& str) {
    std::regex pattern("^\\s+|\\s+$");
    return std::regex_replace(str, pattern, "");
}

bool readIniValue(Config& config) {
    std::ifstream file("config.ini");
    std::string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            if (getline(iss, key, ':')) {
                std::string value;
                if (getline(iss, value)) {
                    key = trim(key);
                    value = trim(value);
                    if (key == "dbc_file") {
                        config.dbc_file = value;
                        continue;
                    } else if (key == "ldf_file") {
                        config.ldf_file = value;
                        continue;
                    } else if (key == "dbc_node") {
                        config.dbc_node = value;
                        continue;
                    } else if (key == "lin_master") {
                        config.lin_master = value == "1" || value == "true";
                        continue;
                    } else if (key == "routinTableId") {
                        config.routinTableId = std::stoi(value);
                        continue;
                    } else if (key == "ldf_node") {
                        config.ldf_node = value;
                        continue;
                    } else if (key == "can_default_values") {
                        parseSignals(value, config.can_signals);
                        continue;
                    } else if (key == "lin_default_values") {
                        parseSignals(value, config.lin_signals);
                        continue;
                    } else {
                        std::cerr << "Unknown key: " << key << std::endl;
                        return false;
                    }
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open config.ini" << std::endl;
    }
    return true;
}