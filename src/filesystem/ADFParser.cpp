#include "engine/Filesystem.h"
#include "engine/ADF.h"
#include <cctype>

ADFEntry::Token ADFEntry::Tokenizer::ReadToken() {
    char currchar;
    std::string TokenContent;

    // Skip any whitespace.
    do {
        currchar = filestream.get();
    } while (std::isspace(currchar));

    // Any of the other cases
    switch (currchar) {
    case '\"':
        TokenContent.reserve(256);

        currchar = filestream.get(); // Has to be like this as to not include the starting quotation mark.
        while (!(currchar == '\"' || currchar == -1)) {
            TokenContent.push_back(currchar);
            currchar = filestream.get();
        }

        TokenContent.shrink_to_fit();

        return Token(TokenType::String, TokenContent);
    case '{':
        return Token(TokenType::StartMap);
    case '[':
        return Token(TokenType::StartArray);
    case '}':
        return Token(TokenType::EndMap);
    case ']':
        return Token(TokenType::EndArray);
    case -1:
        return Token(TokenType::EndFile);
    }

    // Unquoted string
    if (std::isgraph(currchar)) {
        TokenContent.reserve(256);

        do {
            TokenContent.push_back(currchar);
            currchar = filestream.get();
        } while (std::isgraph(currchar));

        TokenContent.shrink_to_fit();

        return Token(TokenType::String, TokenContent);
    }

    // Warning silencer.
    return Token();
}

ADFEntry::ADFEntry(ADFType Type, Tokenizer& Tokenizer) {
    switch (Type) {
    
    case ADFType::map: {
        data = std::map<std::string, ADFEntry>();
        std::map<std::string, ADFEntry>& mapdata = std::get<std::map<std::string, ADFEntry>>(data);

        while (true) {
            Token KeyToken = Tokenizer.ReadToken();
            std::string key;

            switch (KeyToken.type) {
            case TokenType::String:
                key = std::move(KeyToken.content.value());
                break;
            case TokenType::StartMap:
            case TokenType::StartArray:
                Engine::Error("An ADF entry cannot be a key!");
            case TokenType::EndArray:
                Engine::Error("Mismatched ADF closing brackets!(Tried to end a map with a square bracket)");
            case TokenType::EndMap:
            case TokenType::EndFile:
                return;
            }

            Token EntryToken = Tokenizer.ReadToken();

            switch (EntryToken.type) {
                case TokenType::String:
                    mapdata.emplace(std::move(key), String(EntryToken.content.value()));
                break;
                case TokenType::StartMap:
                    mapdata.emplace(std::move(key), ADFEntry(ADFType::map, Tokenizer));
                break;
                case TokenType::StartArray:
                    mapdata.emplace(std::move(key), ADFEntry(ADFType::array, Tokenizer));
                break;
                case TokenType::EndMap:
                case TokenType::EndFile:
                    Engine::Error("Incomplete ADF key/value pair!");
                case TokenType::EndArray:
                    Engine::Error("Incomplete ADF key/value pair!(And also it was closed with a square bracket!)");
            }
        }

        return;
    }
    case ADFType::array: {
        data = std::vector<ADFEntry>();
        std::vector<ADFEntry>& arraydata = std::get<std::vector<ADFEntry>>(data);

        while (true) {
            Token Token = Tokenizer.ReadToken();

            switch (Token.type) {
                case TokenType::String:
                    arraydata.emplace_back(String(Token.content.value()));
                break;
                case TokenType::StartMap:
                    arraydata.emplace_back(ADFEntry(ADFType::map, Tokenizer));
                break;
                case TokenType::StartArray:
                    arraydata.emplace_back(ADFEntry(ADFType::array, Tokenizer));
                break;
                case TokenType::EndArray:
                case TokenType::EndFile:
                    return;
                case TokenType::EndMap:
                    Engine::Error("Mismatched ADF closing brackets!(Tried to end an array with a curly brace)");
            }
        }

        return;
    }
    
    }
}

ENGINEEXPORT ADFEntry ADFEntry::FromFile(std::string FilePath) {
    Tokenizer Tokenizer(FilePath);
    return ADFEntry(ADFType::map, Tokenizer);
}
