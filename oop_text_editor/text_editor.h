#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stack>
#include "line.h"

struct UndoRedoState {
    std::vector<std::unique_ptr<Line>> linesSnapshot;
};

class Cursor {
private: int indexLine, index;
public:
    Cursor(int l, int i) : indexLine(l), index(i) {}
    ~Cursor() = default;

    int getLine() {
        return indexLine;
    }

    int getIndex() {
        return index;
    }

    void setLine(int l) { indexLine = l; }
    void setIndex(int i) { index = i; }
};

class TextEditor {
private:
    std::vector<std::unique_ptr<Line>> lines;

    std::string copy;

    std::stack<UndoRedoState> undoStack;
    std::stack<UndoRedoState> redoStack;

    UndoRedoState saveState();

public:
    TextEditor() = default;
    ~TextEditor() = default;

    Cursor cursor;

    void newLine(std::unique_ptr<Line> nLine);
    void Append();
    void InsertPasteReplace(int choice);
    void Search();
    void DeleteAndCut(bool cut);
    void Copy();
    void printAll() const;

    void Undo();
    void Redo();

    void Save(const std::string& filename);
    void Load(const std::string& filename);

    void RightAndLeft(bool side);
    void UpAndDown(bool side);
    bool PrintWithCursor();
    void ReadingConsole();
};