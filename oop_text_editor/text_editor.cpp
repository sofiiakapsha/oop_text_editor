#include "text_editor.h"
#include "line.h"
#include <iostream>
#include <conio.h>

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

void TextEditor::printAll() const {
    for (const auto& line : lines) {
        line->print();
    }
}