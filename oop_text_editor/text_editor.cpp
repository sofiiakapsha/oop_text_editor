#include "text_editor.h"
#include "text.h"
#include "line.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <fstream>

void TextEditor::RightAndLeft(bool side) {
    int lIndex = cursor.getLine();
    if (lIndex < 0 || lIndex >= static_cast<int>(textStorage.sizeLines())) return;

    Line* curLine = textStorage.findLine(cursor.getLine());
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
    int lastLine = textStorage.sizeLines();

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
    if (textStorage.isEmpty()) {
        std::cout << "No current text.\n";
        return false;
    }

    std::cout << "\n";
    int curLine = cursor.getLine();
    int curIdx = cursor.getIndex();

    for (int i = 0; i < textStorage.sizeLines(); i++) {
        std::string text = textStorage.lineText(i);

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

void TextEditor::AppendW() {
    std::cout << "Enter text: " << std::endl;
    std::string input;
    std::getline(std::cin, input);

    int lIndex = cursor.getLine();

    textStorage.Append(input, lIndex);
}

void TextEditor::InsertPasteReplaceW(int choice) {
    std::printf("Choose line and index:\n");
    ReadingConsole();

    std::string text;
    int line = cursor.getLine();
    int index = cursor.getIndex();

    if (choice == 1 || choice == 3) {
        std::printf(choice == 1 ? "Enter text to insert:\n" : "Enter text to replace:\n");
        std::getline(std::cin, text);
        textStorage.InsertPasteReplace(choice, line, index, text);
    }
    else if (choice == 2) {
        textStorage.InsertPasteReplace(choice, line, index);
    }
    else {
        printf("Invalid command.\n");
        return;
    }
}

void TextEditor::SearchW() {
    std::string text;

    std::printf("Enter text to search:\n");
    std::getline(std::cin, text);

    if (text.size() <= 0) { printf("No searching words entered.\n"); return; }

    textStorage.Search(text);
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

    textStorage.DeleteAndCut(cut, line, indexStart, indexEnd);
}

void TextEditor::CopyW() {
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

    textStorage.Copy(line, indexStart, indexEnd);
}

void TextEditor::SaveW() {
    std::string name;

    std::printf("Enter name for file:\n");
    std::getline(std::cin, name);

    if (name.size() <= 0) { printf("No file name entered.\n"); return; }

    textStorage.Save(name);
}
void TextEditor::LoadW() {

    std::string name;

    std::printf("Enter name for file:\n");
    std::getline(std::cin, name);

    if (name.size() <= 0) { printf("No file name entered.\n"); return; }

    textStorage.Load(name);
}

void TextEditor::mConsole() {
    int choice;

    while (true) {
        std::printf("\nMenu\n");
        std::printf("%-25s %-25s %-25s %-25s\n", "1. Add new line", "2. Append text", "3. Insert text", "4. Paste");
        std::printf("%-25s %-25s %-25s %-25s\n", "5. Replace text", "6. Search", "7. Delete", "8. Cut");
        std::printf("%-25s %-25s %-25s %-25s\n", "9. Copy", "10. Undo", "11. Redo", "12. Print all");
        std::printf("%-25s %-25s %-25s\n", "13. Save", "14. Load", "0. Exit");
        std::printf("Enter choice: ");
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::printf("Invalid input.\n");
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
        case 1: {
            std::printf("Enter line type (1 - Text, 2 - Contact, 3 - Checklist): ");
            int type;
            std::cin >> type;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (type == 1) {
                std::string text;
                std::printf("Enter text: ");
                std::getline(std::cin, text);
                textStorage.newLine(std::make_unique<TextLine>(text));
            }
            else if (type == 2) {
                std::string name, surname, email;
                std::printf("Enter name: "); std::getline(std::cin, name);
                std::printf("Enter surname: "); std::getline(std::cin, surname);
                std::printf("Enter email: "); std::getline(std::cin, email);
                textStorage.newLine(std::make_unique<ContactLine>(name, surname, email));
            }
            else if (type == 3) {
                std::string item;
                int checked;
                std::printf("Enter item: "); std::getline(std::cin, item);
                std::printf("Checked? (1/0): "); std::cin >> checked;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                textStorage.newLine(std::make_unique<ChecklistLine>(item, checked == 1));
            }
            else {
                std::printf("Invalid type.\n");
            }
            break;
        }
        case 2: AppendW(); break;
        case 3: InsertPasteReplaceW(1); break;
        case 4: InsertPasteReplaceW(2); break;
        case 5: InsertPasteReplaceW(3); break;
        case 6: SearchW(); break;
        case 7: DeleteAndCut(false); break;
        case 8: DeleteAndCut(true); break;
        case 9: CopyW(); break;
        case 10: textStorage.Undo(); break;
        case 11: textStorage.Redo(); break;
        case 12: textStorage.printAll(); break;
        case 13: SaveW(); break;
        case 14: LoadW(); break;
        case 0:
            std::printf("Goodbye!\n");
            return;
        default:
            std::printf("Unknown command.\n");
            break;
        }
    }
}

