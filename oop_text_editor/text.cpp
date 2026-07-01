#include "text.h"
#include "line.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <fstream>

UndoRedoState Text::saveState() {
    UndoRedoState state;

    for (const auto& line : lines) {
        state.linesSnapshot.push_back(line->clone());
    }

    return state;
}

void Text::newLine(std::unique_ptr<Line> nLine) {
    UndoRedoState stateBefore = saveState();
    undoStack.push(std::move(stateBefore));

    lines.push_back(std::move(nLine));
    std::printf("New line added\n");

    UndoRedoState stateAfter = saveState();
    redoStack.push(std::move(stateAfter));
}

void Text::Append(std::string input, int lIndex) {

    if (lines.empty()) {
        std::cout << "No current lines.\n";
        return;
    }

    Line* curLine = lines[lIndex].get();
    curLine->append(input);

    std::printf("Text appended.\n");
    while (!redoStack.empty()) redoStack.pop();
}

void Text::InsertPasteReplace(int choice, int line, int index, std::string text) {

    if (choice == 2) {
        if (!copy.empty()) text = copy;
        else { printf("Buffer is empty.\n"); return; }
    }

    if (line < 0 || line >= (int)lines.size()) {
        printf("Error: this line does not exist.\n");
        return;
    }

    std::string t_text = lines[line]->getText();

    if (index < 0 || index >(int)t_text.size()) {
        printf("This index does not exist.\n");
        return;
    }

    UndoRedoState stateBefore = saveState();
    undoStack.push(std::move(stateBefore));

    if (choice == 1 || choice == 2) {
        t_text.insert(index, text);
    }
    else if (choice == 3) {
        size_t count = std::min(text.size(), t_text.size() - index);
        t_text.replace(index, count, text);
    }

    lines[line]->setText(t_text);
    std::printf("Operation is completed.\n");
    while (!redoStack.empty()) redoStack.pop();
}

void Text::Search(std::string text) {
    std::vector<std::pair<int, int>> places;

    for (int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++) {
        std::string t_text = lines[lineIndex]->getText();

        size_t pos = t_text.find(text, 0);
        while (pos != std::string::npos) {
            places.push_back({ lineIndex, (int)pos });
            pos = t_text.find(text, pos + 1);
        }
    }

    if (places.empty()) {
        printf("Text not found.\n");
        return;
    }

    for (const auto& p : places) {
        std::printf("Found at line %d, index %d\n", p.first, p.second);
    }
}

void Text::DeleteAndCut(bool cut, int line, int indexStart, int indexEnd) {

    int number = indexEnd - indexStart;

    if (number <= 0) {
        std::printf("You cannot delete this info.\n");
        return;
    }

    if (line < 0 || line >= (int)lines.size()) {
        printf("This line does not exist.\n");
        return;
    }

    std::string text = lines[line]->getText();

    if (indexStart < 0 || indexEnd >(int)text.size()) {
        printf("Invalid index range.\n");
        return;
    }

    UndoRedoState stateBefore = saveState();
    undoStack.push(std::move(stateBefore));

    if (cut) {
        copy = text.substr(indexStart, number);
    }

    text.erase(indexStart, number);
    lines[line]->setText(text);

    printf("Information deleted.\n");
    while (!redoStack.empty()) redoStack.pop();
}

void Text::Copy(int line, int indexStart, int indexEnd) {

    int number = indexEnd - indexStart;

    if (number <= 0) {
        std::printf("You cannot copy this info.\n");
        return;
    }

    if (line < 0 || line >= (int)lines.size()) {
        printf("This line does not exist.\n");
        return;
    }

    std::string text = lines[line]->getText();

    if (indexStart < 0 || indexEnd >(int)text.size()) {
        printf("Invalid index range.\n");
        return;
    }

    copy = text.substr(indexStart, number);

    std::printf("Copied successfully.\n");
}

void Text::Undo() {
    if (undoStack.empty()) {
        printf("Nothing to undo.\n");
        return;
    }

    UndoRedoState undoVersion = std::move(undoStack.top());
    undoStack.pop();

    UndoRedoState currentState = saveState();
    redoStack.push(std::move(currentState));

    lines = std::move(undoVersion.linesSnapshot);

    printf("Undo performed.\n");
}

void Text::Redo() {
    if (redoStack.empty()) {
        printf("Nothing to redo.\n");
        return;
    }

    UndoRedoState redoVersion = std::move(redoStack.top());
    redoStack.pop();

    UndoRedoState currentState = saveState();
    undoStack.push(std::move(currentState));

    lines = std::move(redoVersion.linesSnapshot);

    printf("Redo performed.\n");
}

void Text::Save(const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        printf("Could not open file for writing.\n");
        return;
    }

    for (const auto& line : lines) {
        file << line->serialize() << "\n";
    }

    file.close();
    printf("File saved successfully.\n");
}

void Text::Load(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        printf("Could not open file for reading.\n");
        return;
    }

    lines.clear();

    std::string loadString;
    while (std::getline(file, loadString)) {
        std::unique_ptr<Line> loadLine = Line::deserialize(loadString);
        lines.push_back(std::move(loadLine));
    }

    file.close();
    printf("File loaded successfully.\n");
}


std::vector<std::string> Text::allSerialized() {
    std::vector<std::string> result;
    for (const auto& line : lines)
        result.push_back(line->serialize());
    return result;
}

void Text::replaceWithRaw(const std::vector<std::string>& rawLines) {
    lines.clear();
    for (const auto& s : rawLines)
        lines.push_back(std::make_unique<TextLine>(s));
}

void Text::loadFromSerialized(const std::vector<std::string>& serializedLines) {
    lines.clear();
    for (const auto& s : serializedLines) {
        auto line = Line::deserialize(s);
        if (line)
            lines.push_back(std::move(line));
        else
            lines.push_back(std::make_unique<TextLine>(s));
    }
}

void Text::printAll() const {
    for (const auto& line : lines) {
        line->print();
    }
}