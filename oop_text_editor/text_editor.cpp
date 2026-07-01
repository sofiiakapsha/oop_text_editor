#include "text_editor.h"
#include "cipher.h"
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

    if (!curLine->hasColumns()) {
        cursor.setIndex(0);
        return;
    }

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
    int step = side ? -1 : 1;
    int next = lineInd + step;

    while (next >= 0 && next < lastLine && !textStorage.findLine(next)->hasColumns()) {
        next += step;
    }

    if (next >= 0 && next < lastLine) {
        cursor.setLine(next);
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

        if (i != curLine || !textStorage.findLine(i)->hasColumns()) {
            std::cout << text << std::endl;
        }
        else {
            for (size_t j = 0; j < text.size(); j++) {
                if ((int)j == curIdx) std::cout << "|";
                std::cout << text[j];
            }
            if (curIdx == (int)text.size()) std::cout << "|";
            std::cout << "\n";
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
    std::printf("Choose line and index:\n");
    ReadingConsole();

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

void TextEditor::EncryptFileW() {
    std::string inputPath, outputPath, key;
    int cipherType;

    std::printf("Enter input file path: ");
    std::getline(std::cin, inputPath);
    if (inputPath.empty()) { std::printf("No path entered.\n"); return; }
    textStorage.Load(inputPath);

    std::printf("Enter output file path: ");
    std::getline(std::cin, outputPath);
    if (outputPath.empty()) { std::printf("No path entered.\n"); return; }

    std::printf("Choose cipher: 1 - Caesar, 2 - Vigenere, 3 - Atbash\n");
    std::printf("Enter choice: ");
    if (!(std::cin >> cipherType) || cipherType < 1 || cipherType > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::printf("Invalid cipher choice.\n");
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (cipherType == 1) {
        std::printf("Enter Caesar key (integer): ");
        std::getline(std::cin, key);
    }
    else if (cipherType == 2) {
        std::printf("Enter Vigenere key (word): ");
        std::getline(std::cin, key);
    }

    Cipher cipherTool;
    cipherTool.Encrypt(textStorage, cipherType, key);
    textStorage.Save(outputPath);
}

void TextEditor::DecryptFileW() {
    std::string inputPath, outputPath, key;
    int cipherType;

    std::printf("Enter input file path: ");
    std::getline(std::cin, inputPath);
    if (inputPath.empty()) { std::printf("No path entered.\n"); return; }
    textStorage.Load(inputPath);

    std::printf("Enter output file path: ");
    std::getline(std::cin, outputPath);
    if (outputPath.empty()) { std::printf("No path entered.\n"); return; }

    std::printf("Choose cipher: 1 - Caesar, 2 - Vigenere, 3 - Atbash\n");
    std::printf("Enter choice: ");
    if (!(std::cin >> cipherType) || cipherType < 1 || cipherType > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::printf("Invalid cipher choice.\n");
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (cipherType == 1) {
        std::printf("Enter Caesar key (integer): ");
        std::getline(std::cin, key);
    }
    else if (cipherType == 2) {
        std::printf("Enter Vigenere key (word): ");
        std::getline(std::cin, key);
    }

    Cipher cipherTool;
    cipherTool.Decrypt(textStorage, cipherType, key);
    textStorage.Save(outputPath);
}

void TextEditor::ToggleChecklistW() {
    printf("Choose checklist line:\n");
    ReadingConsole();
    textStorage.ToggleChecklist(cursor.getLine());
}

void TextEditor::mConsole() {
    int choice;

    while (true) {
        std::printf("\nMenu\n");
        std::printf("%-25s %-25s %-25s %-25s\n", "1. Add new line", "2. Append text", "3. Insert text", "4. Paste");
        std::printf("%-25s %-25s %-25s %-25s\n", "5. Replace text", "6. Search", "7. Delete", "8. Cut");
        std::printf("%-25s %-25s %-25s %-25s\n", "9. Copy", "10. Undo", "11. Redo", "12. Print all");
        std::printf("%-25s %-25s %-25s %-25s\n", "13. Save", "14. Load", "15. Encrypt Text", "16. Decrypt Text");
        std::printf("%-25s %-25s\n", "17. Toggle", "0. Exit");
        std::printf("0. Exit\n");
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

                std::printf("Enter name: ");
                std::getline(std::cin, name);
                if (name.empty()) {
                    std::printf("Error: name cannot be empty.\n");
                    break;
                }

                std::printf("Enter surname: ");
                std::getline(std::cin, surname);
                if (surname.empty()) {
                    std::printf("Error: surname cannot be empty.\n");
                    break;
                }

                std::printf("Enter email: ");
                std::getline(std::cin, email);
                if (email.empty()) {
                    std::printf("Error: email cannot be empty.\n");
                    break;
                }
                if (email.find('@') == std::string::npos) {
                    std::printf("Error: email must contain '@'.\n");
                    break;
                }

                textStorage.newLine(std::make_unique<ContactLine>(name, surname, email));
            }
            else if (type == 3) {
                std::string item;
                int checked;
                std::printf("Enter item: "); std::getline(std::cin, item);
                if (item.empty()) {
                    std::printf("Error: item cannot be empty.\n");
                    break;
                }
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
        case 15: EncryptFileW(); break;
        case 16: DecryptFileW(); break;
        case 17: ToggleChecklistW(); break;
        case 0:
            std::printf("Goodbye!\n");
            return;
        default:
            std::printf("Unknown command.\n");
            break;
        }
    }
}
