#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stack>
#include "line.h"

struct UndoRedoState {
    std::vector<std::unique_ptr<Line>> linesSnapshot;
};

class Text {
private:
    std::vector<std::unique_ptr<Line>> lines;

    std::string copy;

    std::stack<UndoRedoState> undoStack;
    std::stack<UndoRedoState> redoStack;

    UndoRedoState saveState();

public:
    Text() = default;
    ~Text() = default;

    int sizeLines() { return (int)lines.size(); }
    Line* findLine(int l) { return lines[l].get(); }
    bool isEmpty() { return lines.empty(); }
    std::string lineText(int i) { return lines[i]->getText(); }

    void newLine(std::unique_ptr<Line> nLine);
    void Append(std::string input, int lIndex);
    void InsertPasteReplace(int choice, int line, int index, std::string text = NULL);
    void Search(std::string text);
    void DeleteAndCut(bool cut, int line, int indexStart, int indexEnd);
    void Copy(int line, int indexStart, int indexEnd);
    void printAll() const;

    void Undo();
    void Redo();

    void Save(const std::string& filename);
    void Load(const std::string& filename);
};