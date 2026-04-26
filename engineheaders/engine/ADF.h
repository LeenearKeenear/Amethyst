#pragma once
#include "engine/master.h"
#include <variant>
#include <optional>
#include <map>
#include <string>
#include <vector>

class ADFEntry {
    enum class ADFType {
        map,
        array
    };
    enum class TokenType {
        String,
        StartMap,
        StartArray,
        EndMap,
        EndArray,
        EndFile
    };
    struct Token {
        TokenType type;
        std::optional<std::string> content;
        inline Token() {}
        inline Token (TokenType Type, std::optional<std::string> Content = std::nullopt) { type = Type; content = Content; }
    };

    std::variant<std::map<std::string, ADFEntry>, std::string, std::vector<ADFEntry>> data;


    class Tokenizer {
        std::ifstream filestream;
    public:
        inline Tokenizer(std::string FilePath) {
            filestream = Filesystem::GetFile(FilePath, std::ios::in | std::ios_base::binary);
        }
        Token ReadToken();
    };


    ADFEntry(ADFType Type, Tokenizer& Tokenizer);
    ADFEntry() {};
public:
    //! Creates an ADF tree from a .adf file.
    static ENGINEEXPORT ADFEntry FromFile(std::string FilePath);

    //! Used for manual adding of string-type entries.
    inline static ADFEntry String(std::string Content = std::string()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual adding of map-type entries.
    inline static ADFEntry Map(std::map<std::string, ADFEntry> Content = std::map<std::string, ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }
    //! Used for manual adding of array-type entries.
    inline static ADFEntry Array(std::vector<ADFEntry> Content = std::vector<ADFEntry>()) { ADFEntry ret; ret.data = Content; return ret; }

    inline std::string& GetString() {
        if (!std::holds_alternative<std::string>(data)) {
            Engine::Error("Tried to get a string value from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    inline std::map<std::string, ADFEntry>& GetChildren() {
        if (!std::holds_alternative<std::map<std::string, ADFEntry>>(data)) {
            Engine::Error("Tried to get a list of children from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    inline std::vector<ADFEntry>& GetArray() {
        if (!std::holds_alternative<std::vector<ADFEntry>>(data)) {
            Engine::Error("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }

    inline const std::string& GetString() const {
        if (!std::holds_alternative<std::string>(data)) {
            Engine::Error("Tried to get a string value from a different type of an ADF entry!");
        }
        return std::get<std::string>(data);
    }
    inline const std::map<std::string, ADFEntry>& GetChildren() const {
        if (!std::holds_alternative<std::map<std::string, ADFEntry>>(data)) {
            Engine::Error("Tried to get a list of children from a different type of an ADF entry!");
        }
        return std::get<std::map<std::string, ADFEntry>>(data);
    }
    inline const std::vector<ADFEntry>& GetArray() const {
        if (!std::holds_alternative<std::vector<ADFEntry>>(data)) {
            Engine::Error("Tried to get an array from a different type of an ADF entry!");
        }
        return std::get<std::vector<ADFEntry>>(data);
    }



    inline ADFEntry& operator[](int i) {
        return GetArray()[i];
    }

    inline ADFEntry& operator[](const std::string& name) {
        return GetChildren().at(name);
    }

    inline const ADFEntry& operator[](int i) const {
        return GetArray()[i];
    }

    inline const ADFEntry& operator[](const std::string& name) const {
        return GetChildren().at(name);
    }

    inline bool HasChild(const std::string& name) const {
        return GetChildren().contains(name);
    }

};