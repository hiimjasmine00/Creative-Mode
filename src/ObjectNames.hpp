#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ObjectNames {
    public:
    std::unordered_map<int, std::string> m_names;

    ObjectNames() {

        std::filesystem::path objectNames = Mod::get()->getResourcesDir() / "objects.csv";
        if (!std::filesystem::exists(objectNames)) return;

        std::ifstream inputFile(objectNames);

        std::string value;

        if (inputFile.is_open()) {
            while (getline(inputFile, value)) {
                std::vector<std::string> split = utils::string::split(value, ",");
                if (split.size() == 2) {
                    auto resID = utils::numFromString<int>(split[0]);
                    if (resID.isOk()) {
                        m_names[resID.unwrap()] = split[1];
                    }
                }
            }
            inputFile.close();
        }
    }

    std::string nameForID(int id) {
        if (m_names.contains(id) && !m_names[id].empty()) return m_names[id];
        return "Unknown";
    }

    static ObjectNames* get() {
        static ObjectNames* instance = nullptr;
        if (!instance) {
            instance = new ObjectNames();
        }
        return instance;
    }

};




