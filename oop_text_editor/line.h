#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>

class Line {
public:
    virtual void print() const = 0;
    virtual std::string serialize() const = 0;
    virtual std::unique_ptr<Line> clone() const = 0;
    virtual void append(const std::string&) {} // нове
    static std::unique_ptr<Line> deserialize(const std::string& serializedData);
    virtual ~Line() {}
};

class TextLine : public Line {
    std::string data;
public:
    TextLine(const std::string& text = "") : data(text) {}

    void print() const override {
        std::cout << "Text: " << data << "\n";
    }

    std::unique_ptr<Line> clone() const override {
        return std::make_unique<TextLine>(data);
    }

    std::string serialize() const override {
        return "TEXT|" + data;
    }
    
    void append(const std::string& text) override {
        data += text;
    }
};

class ContactLine : public Line {
    std::string name, surname, email;
public:
    ContactLine(std::string n, std::string s, std::string e)
        : name(std::move(n)), surname(std::move(s)), email(std::move(e)) {
    }

    void print() const override {
        std::cout << "Contact - " << name << " " << surname << ", E-mail: " << email << "\n";
    }

    std::unique_ptr<Line> clone() const override {
        return std::make_unique<ContactLine>(name, surname, email);
    }

    std::string serialize() const override {
        return "CONT|" + name + "|" + surname + "|" + email;
    }
};

class ChecklistLine : public Line {
    std::string item;
    bool checked;
public:
    ChecklistLine(std::string i, bool c) : item(std::move(i)), checked(c) {}

    void print() const override {
        std::cout << "[ " << (checked ? "x" : " ") << " ] " << item << "\n";
    }

    std::unique_ptr<Line> clone() const override {
        return std::make_unique<ChecklistLine>(item, checked);
    }

    std::string serialize() const override {
        return "CHEK|" + item + "|" + (checked ? "1" : "0");
    }
};

inline std::unique_ptr<Line> Line::deserialize(const std::string& serialized_data) {
    if (serialized_data.empty()) return nullptr;

    std::stringstream ss(serialized_data);
    std::string typeMarker;

    std::getline(ss, typeMarker, '|');

    if (typeMarker == "TEXT") {
        std::string text;
        std::getline(ss, text);
        return std::make_unique<TextLine>(text);
    }
    else if (typeMarker == "CONT") {
        std::string name, surname, email;
        std::getline(ss, name, '|');
        std::getline(ss, surname, '|');
        std::getline(ss, email);
        return std::make_unique<ContactLine>(name, surname, email);
    }
    else if (typeMarker == "CHEK") {
        std::string item, checkedStr;
        std::getline(ss, item, '|');
        std::getline(ss, checkedStr);
        bool checked = (checkedStr == "1");
        return std::make_unique<ChecklistLine>(item, checked);
    }

    return nullptr;
}