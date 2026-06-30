#include "text_editor.h"
#include "line.h"
#include <iostream>
#include <conio.h>
#include <string>

UndoRedoState TextEditor::saveState() {
    UndoRedoState state;

    for (const auto& line : lines) {
        state.linesSnapshot.push_back(line->clone());
    }

    return state;
}

void TextEditor::RightAndLeft(bool side) {
    int lIndex = cursor.getLine();
    if (lIndex < 0 || lIndex >= static_cast<int>(lines.size())) return;

    Line* curLine = lines[cursor.getLine()].get();
    int curIndex = cursor.getIndex();

    if (side) {
        if (static_cast<int>(curLine->getText().size()) > curIndex) {
            cursor.setIndex(curIndex + 1);
        }
    }
    else if (curIndex - 1 >= 0) {
        cursor.setIndex(curIndex - 1);
    }
}

void TextEditor::UpAndDown(bool side) {
    int lineInd = cursor.getLine();
    int lastLine = static_cast<int>(lines.size());

    if (side) {
        if (lineInd > 0) {
            cursor.setLine(lineInd - 1);
            cursor.setIndex(0);
        }
    }
    else if (lastLine > lineInd + 1) {
        cursor.setLine(lineInd + 1);
        cursor.setIndex(0);
    }
}

bool TextEditor::PrintWithCursor() {
    if (lines.empty()) {
        std::cout << "No current text.\n";
        return false;
    }

    std::cout << "\n";
    int curLine = cursor.getLine();
    int curIdx = cursor.getIndex();

    for (int i = 0; i < (int)lines.size(); i++) {
        std::string text = lines[i]->getText();

        if (i != curLine) {
            std::cout << text;
        }
        else {
            for (size_t j = 0; j < text.size(); j++) {
                if ((int)j == curIdx) std::cout << "|";
                std::cout << text[j];
            }
            if (curIdx == (int)text.size()) std::cout << "|";
        }
    }
    std::cout << "\n";
    return true;
}

void TextEditor::ReadingConsole() {
    int ch1, ch2;
    std::cout << "\n";

    if (!PrintWithCursor()) return;

    while (true) {
        ch1 = _getch();

        if (ch1 == 0xE0 || ch1 == 0) {
            ch2 = _getch();
            switch (ch2) {
            case 72: UpAndDown(true); break;
            case 80: UpAndDown(false); break;
            case 75: RightAndLeft(false); break;
            case 77: RightAndLeft(true); break;
            }
            PrintWithCursor();
        }
        else if (ch1 == 27) break;
    }
}

void TextEditor::newLine(std::unique_ptr<Line> nLine, bool isPrinting) {
    UndoRedoState stateBefore = saveState();
    undoStack.push(std::move(stateBefore));

    lines.push_back(std::move(nLine));
    std::printf("New line added\n");

    UndoRedoState stateAfter = saveState();
    redoStack.push(std::move(stateAfter));
}

void TextEditor::Append() {

    if (lines.empty()) {
        std::cout << "No current lines.\n";
        return;
    }
    UndoRedoState stateBefore = saveState();
    undoStack.push(std::move(stateBefore));

    std::cout << "Enter text: " << std::endl;
    std::string input;
    std::getline(std::cin, input);

    int lIndex = cursor.getLine();

    if (lIndex < 0 || lIndex >= static_cast<int>(lines.size())) {
        lIndex = static_cast<int>(lines.size()) - 1;
    }

    Line* curLine = lines[lIndex].get();
    curLine->append(input);

    std::printf("Text appended.\n");

    UndoRedoState stateAfter = saveState();
    redoStack.push(std::move(stateAfter));
}

void TextEditor::InsertPasteReplace(int choice) {
    std::printf("Choose line and index:\n");
    ReadingConsole();

    std::string text;
    int line = cursor.getLine();
    int index = cursor.getIndex();

    if (choice == 1 || choice == 3) {
        std::printf(choice == 1 ? "Enter text to insert:\n" : "Enter text to replace:\n");
        std::getline(std::cin, text);
    }
    else if (choice == 2) {
        if (!copy.empty()) text = copy;
        else { printf("Buffer is empty.\n"); return; }
    }
    else {
        printf("Invalid command.\n");
        return;
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

    UndoRedoState stateAfter = saveState();
    redoStack.push(std::move(stateAfter));
}

void TextEditor::Search() {
    std::string text;

    std::printf("Enter text to search:\n");
    std::getline(std::cin, text);

    if (text.size() <= 0) { printf("No searching words entered.\n"); return; }
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

void TextEditor::DeleteAndCut(bool cut) {
    printf("Choose line, index for start and then for end (only in one line):\n");
    ReadingConsole();

    int line = cursor.getLine();
    int indexStart = cursor.getIndex();

    ReadingConsole();
    int indexEnd = cursor.getIndex();

    if (line != cursor.getLine()) {
        printf("Only in one line!\n");
        return;
    }

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

    UndoRedoState stateAfter = saveState();
    redoStack.push(std::move(stateAfter));
}

void TextEditor::printAll() const {
    for (const auto& line : lines) {
        line->print();
    }
}