#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stack>
#include "line.h"
#include "text.h"

class Cursor {
private: int indexLine, index;
public:
    Cursor() : indexLine(0), index(0) {}
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
private: Text textStorage;
public:
    TextEditor() = default;
    ~TextEditor() = default;

    Cursor cursor;

    void AppendW();
    void InsertPasteReplaceW(int choice);
    void SearchW();
    void DeleteAndCut(bool cut);
    void CopyW();

    void SaveW();
    void LoadW();

    void EncryptFileW();
    void DecryptFileW();

    void RightAndLeft(bool side);
    void UpAndDown(bool side);
    bool PrintWithCursor();
    void ReadingConsole();

    void mConsole();
};