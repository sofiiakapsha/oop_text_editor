#include "text_editor.h"
#include "line.h"
#include <iostream>

UndoRedoState TextEditor::saveState() {
    UndoRedoState state;

    for (const auto& line : lines) {
        state.linesSnapshot.push_back(line->clone());
    }

    return state;
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