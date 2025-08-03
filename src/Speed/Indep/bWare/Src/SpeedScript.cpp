#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/types.h"

#include <stdarg.h>

// TODO
bool IsWhiteSpace(char c) {}

SpeedScript::SpeedScript(const char *filename, BOOL enable_fatal_error) {
    this->ErrorFunction = nullptr;
    if (enable_fatal_error) {
        this->ErrorFunction = SpeedScript::DefaultErrorFunction;
    }
    this->InitFromFile(filename);
}

void SpeedScript::InitFromFile(const char *filename) {
    int file_size = 0;
    char *buffer = static_cast<char *>(bGetFile(filename, &file_size, 0));
    bFileFlushCachedFiles();
    this->Init(filename, buffer, file_size);
    bFree(buffer);
}

SpeedScript::~SpeedScript() {
    for (int file_num = 0; file_num < this->NumFiles; file_num++) {
        if (this->FileTable[file_num].ArgBuf) {
            delete this->FileTable[file_num].ArgBuf;
        }
    }
    if (this->EntryTable) {
        delete this->EntryTable;
    }
}

void SpeedScript::Error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    bVSPrintf(this->ErrorText, format, &args);
    if (this->ErrorFunction) {
        this->ErrorFunction(this->ErrorText);
    }
    va_end(args);
}

void SpeedScript::DefaultErrorFunction(const char *msg) {
    bBreak();
}

char *SpeedScript::GetPositionName() {
    static char text[256];
    SpeedScriptEntry *entry = this->GetCurrentEntry();

    if (entry) {
        bSPrintf(text, "line %d in %s", entry->LineNumber, &this->FileTable[entry->FileNumber]);
    } else {
        bSPrintf(text, "%s", this->FileTable);
    }
    return text;
}

void SpeedScript::ResizeEntryTable(int new_size) {
    SpeedScriptEntry *new_table = new SpeedScriptEntry[new_size];
    if (this->EntryTable) {
        bMemCpy(new_table, this->EntryTable, this->NumEntries * sizeof(SpeedScriptEntry));
        if (this->EntryTable) {
            delete this->EntryTable;
        }
    }
    this->EntryTable = new_table;
    this->MaxEntries = new_size;
}

SpeedScriptEntry *SpeedScript::AddEntry() {
    if (this->NumEntries == this->MaxEntries) {
        this->ResizeEntryTable((this->NumEntries * 4) / 3 + 1);
    }
    SpeedScriptEntry *entry = &this->EntryTable[this->NumEntries];
    this->NumEntries++;
    bMemSet(entry, 0, sizeof(SpeedScriptEntry));
    return entry;
}

bool SpeedScript::ParseNextWord(char *word, const char *buffer, int buffer_size, int *pbuffer_pos, int *pline_number) {
    int buffer_pos = *pbuffer_pos;
    bool currently_in_comment = false;

    do {
        char c = buffer_pos < buffer_size ? buffer[buffer_pos] : '\0';
        if (c == '\r') {
            buffer_pos++;
            c = buffer_pos < buffer_size ? buffer[buffer_pos] : '\0';
        }
        if (c == '\0') {
            *pbuffer_pos = buffer_pos;
            return false;
        }
        if (c == '\n') {
            currently_in_comment = false;
            (*pline_number)++;
        }
        if (!currently_in_comment) {
            if ((c == '/') && (buffer[buffer_pos + 1] == '/')) {
                currently_in_comment = true;
            } else if (!IsWhiteSpace(c)) {
                break;
            }
        }
        buffer_pos++;
    } while (true);

    bool is_in_quotes = false;
    int word_length = 0;

    for (; buffer_pos < buffer_size; buffer_pos++) {
        char c = buffer[buffer_pos];
        if ((c != '\0') && (is_in_quotes || !IsWhiteSpace(c))) {
            if (c == '\"') {
                if (is_in_quotes && (buffer[buffer_pos + 1] == '\"')) {
                    word[word_length] = '\"';
                    buffer_pos++;
                    word_length++;
                } else {
                    is_in_quotes = !is_in_quotes;
                }
            } else {
                word[word_length] = c;
                word_length++;
            }
        } else {
            break;
        }
    }
    word[word_length] = '\0';
    *pbuffer_pos = buffer_pos;
    return true;
}

void SpeedScript::Init(const char *name, const char *buffer, int buffer_size) {
    this->NumFiles = 1;
    SpeedScriptFile *file = &this->FileTable[0];
    this->ErrorText[0] = '\0';
    bStrCpy(file->Filename, name);
    file->ArgBuf = new char[buffer_size + 1];
    this->NumEntries = 0;
    this->MaxEntries = 0;
    this->EntryTable = nullptr;
    this->ResizeEntryTable(buffer_size / 16 + 32);
    this->NextEntryNum = 0;
    int buffer_pos = 0;
    int arg_buf_pos = 0;
    int line_number = 1;
    while (this->ParseNextWord(&file->ArgBuf[arg_buf_pos], buffer, buffer_size, &buffer_pos, &line_number)) {
        char *word = &file->ArgBuf[arg_buf_pos];
        int len = bStrLen(word);

        SpeedScriptEntry *entry = this->AddEntry();
        entry->LineNumber = line_number;
        entry->ArgBufPos = arg_buf_pos;
        if (word[len - 1] == ':') {
            entry->IsCommand = true;
            for (int n = 0; n < len; n++) {
                word[n] = bToUpper(word[n]);
            }
        }
        arg_buf_pos += len + 1;
        if (this->NumEntries > 1) {
            SpeedScriptEntry *prev_entry = &entry[-1];
            if (!entry->IsCommand && prev_entry->IsCommand) {
                if (bStrCmp(this->GetName(prev_entry), "INCLUDESCRIPT:") == 0) {
                    char *filename = this->GetName(entry);
                    this->HandleIncludeScript(filename);
                }
            }
        }
    }
    int prev_command_num = this->NumEntries;
    for (int entry_num = prev_command_num - 1; entry_num >= 0; entry_num--) {
        this->EntryTable[entry_num].NextCommandOffset = prev_command_num - entry_num;
        if (this->EntryTable[entry_num].IsCommand) {
            prev_command_num = entry_num;
        }
    }
}

void SpeedScript::HandleIncludeScript(const char *filename) {
    SpeedScript script(filename, 1);

    if (this->NumFiles + script.NumFiles > 4) {
        this->Error("Too many nested INCLUDESCRIPT commands at %s\n", this->GetPositionName());
    } else {
        for (int n = 0; n < script.NumEntries; n++) {
            SpeedScriptEntry *entry = this->AddEntry();
            *entry = script.EntryTable[n];
            entry->FileNumber += this->NumFiles;
        }

        for (int file_num = 0; file_num < script.NumFiles; file_num++) {
            this->FileTable[this->NumFiles + file_num] = script.FileTable[file_num];
        }
        this->NumFiles += script.NumFiles;
        script.NumFiles = 0;
    }
}

char *SpeedScript::GetNextCommand() {
    SpeedScriptEntry *entry = this->GetNextEntry();

    if (!entry) {
        return nullptr;
    }
    if (!entry->IsCommand) {
        this->NextEntryNum += entry->NextCommandOffset;
        entry = this->GetNextEntry();
        if (!entry) {
            return nullptr;
        }
    }
    this->NextEntryNum++;
    return this->GetName(entry);
}

// UNSOLVED
char *SpeedScript::GetNextCommand(const char *command) {
    char *s;

    do {
        s = this->GetNextCommand();
        if (!s) {
            return nullptr;
        }
    } while (bStrICmp(s, command) != 0);
    return s;
}

// TODO fake match, isArg doesn't exist
bool SpeedScript::IsAnotherArgument() {
    SpeedScriptEntry *entry = this->GetNextEntry();
    bool isArg = false;
    if (entry) {
        isArg = !entry->IsCommand;
    }
    return isArg;
}

char *SpeedScript::GetNextArgument() {
    SpeedScriptEntry *entry = this->GetNextEntry();

    if (entry && !entry->IsCommand) {
        this->NextEntryNum++;
        return this->GetName(entry);
    }
    return nullptr;
}

char *SpeedScript::GetNextArgumentString() {
    char *arg = this->GetNextArgument();
    if (!arg) {
        this->Error("Not enough arguments in %s\n", this->GetPositionName());
    }
    return arg;
}

int SpeedScript::GetNextArgumentInt() {
    char *s = this->GetNextArgumentString();
    if (s) {
        if ((s[0] == '0') && (s[1] == 'x')) {
            int hex = bStrToLong(s);
            return hex;
        }
        if (bIsDigit(s[0]) || (s[0] == '-')) {
            int dec = bStrToLong(s);
            return dec;
        }
        this->Error("Expected a number but found %s in %s\n", s, this->GetPositionName());
    }
    return 0;
}

short SpeedScript::GetNextArgumentShort() {
    int a = this->GetNextArgumentInt();
    if (a + 0x8000U > 0x17fff) {
        this->Error("Parameter %d wouldn\'t fit into short in %s\n", a, this->GetPositionName());
    }
    return a;
}

float SpeedScript::GetNextArgumentFloat() {
    char *s = this->GetNextArgumentString();
    float value = bStrToFloat(s);
    return value;
}

bVector3 SpeedScript::GetNextArgumentVector3() {
    float x = this->GetNextArgumentFloat();
    float y = this->GetNextArgumentFloat();
    float z = this->GetNextArgumentFloat();
    return bVector3(x, y, z);
}
