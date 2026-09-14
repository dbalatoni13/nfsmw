#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "types.h"

#include <stdarg.h>

bool IsWhiteSpace(char c) {
    if (c != ' ') {
        if (c != '\n' && c != '\t' && c != '=' && c != ',') {
            return c == '\r';
        }
    }
    return true;
}

SpeedScript::SpeedScript(const char *filename, BOOL enable_fatal_error) {
    this->ErrorFunction = nullptr;
    if (enable_fatal_error) {
        this->ErrorFunction = SpeedScript::DefaultErrorFunction;
    }
    this->InitFromFile(filename);
}

SpeedScript::SpeedScript(const char *script_name, const char *text_buffer, int enable_fatal_error) {
    this->ErrorFunction = enable_fatal_error ? SpeedScript::DefaultErrorFunction : nullptr;
    this->Init(script_name, text_buffer, bStrLen(text_buffer));
}

SpeedScript::SpeedScript(const char *filename, void (*error_function)(const char *)) {
    this->ErrorFunction = error_function;
    this->InitFromFile(filename);
}

SpeedScript::SpeedScript(const char *script_name, const char *text_buffer, void (*error_function)(const char *)) {
    this->ErrorFunction = error_function;
    this->Init(script_name, text_buffer, bStrLen(text_buffer) + 1);
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
        delete[] this->FileTable[file_num].ArgBuf;
    }
    if (this->EntryTable) {
        delete[] this->EntryTable;
    }
}

void SpeedScript::Error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    bVSPrintf(this->ErrorText, format, args);
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
        delete[] this->EntryTable;
    }
    this->EntryTable = new_table;
    this->MaxEntries = new_size;
}

SpeedScriptEntry *SpeedScript::AddEntry() {
    if (this->MaxEntries == this->NumEntries) {
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
            (*pline_number)++;
            currently_in_comment = false;
        }
        if (!currently_in_comment) {
            if ((c == '/') && (buffer[buffer_pos + 1] == '/')) {
                currently_in_comment = true;
            } else if (c != ' ' && c != '\n' && c != '\t' && c != '=' && c != ',' && c != '\r') {
                break;
            }
        }
        buffer_pos++;
    } while (true);

    bool is_in_quotes = false;
    int word_length = 0;

    for (; buffer_pos < buffer_size; buffer_pos++) {
        char c = buffer[buffer_pos];
        if ((c != '\0') &&
            (is_in_quotes || (c != ' ' && c != '\n' && c != '\t' && c != '=' && c != ',' && c != '\r'))) {
            if (c == '\"') {
                if (is_in_quotes && (buffer[buffer_pos + 1] == '\"')) {
                    word[word_length] = '\"';
                    word_length++;
                    buffer_pos++;
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
    int script_size = buffer_size;
    this->ErrorText[0] = '\0';
    this->NumFiles = 1;
    bStrCpy(this->FileTable[0].Filename, name);
    SpeedScriptFile *file = &this->FileTable[0];
    file->ArgBuf = new char[script_size + 1];
    this->NumEntries = 0;
    this->MaxEntries = 0;
    this->EntryTable = nullptr;
    this->ResizeEntryTable(script_size / 16 + 32);
    this->NextEntryNum = 0;
    int buffer_pos = 0;
    int arg_buf_pos = 0;
    int line_number = 1;
    while (this->ParseNextWord(&file->ArgBuf[arg_buf_pos], buffer, script_size, &buffer_pos, &line_number)) {
        char *word = &file->ArgBuf[arg_buf_pos];
        int len = bStrLen(word);

        if (this->NumEntries == this->MaxEntries) {
            this->ResizeEntryTable((this->MaxEntries * 4) / 3 + 1);
        }
        SpeedScriptEntry *entry = &this->EntryTable[this->NumEntries++];
        bMemSet(entry, 0, sizeof(SpeedScriptEntry));
        entry->LineNumber = line_number;
        entry->ArgBufPos = arg_buf_pos;
        if (word[len - 1] == ':') {
            entry->IsCommand = true;
            for (int n = 0; n < len; n++) {
                word[n] = bToUpper(word[n]);
            }
        }
        arg_buf_pos += len + 1;
        if (this->NumEntries >= 2) {
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
            if (this->NumEntries == this->MaxEntries) {
                this->ResizeEntryTable((this->MaxEntries * 4) / 3 + 1);
            }
            SpeedScriptEntry *entry = &this->EntryTable[this->NumEntries++];
            bMemSet(entry, 0, sizeof(SpeedScriptEntry));
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

char *SpeedScript::GetNextCommand(const char *command) {
    char *s;
    while ((s = GetNextCommand()) != nullptr) {
        if (bStrICmp(s, command) == 0) {
            return s;
        }
    }
    return nullptr;
}

char *SpeedScript::PeekNextCommand() {
    const int saved_position = this->NextEntryNum;
    char *command = this->GetNextCommand();
    this->NextEntryNum = saved_position;
    return command;
}

char *SpeedScript::GetCommandArgument(const char *command) {
    if (this->GetNextCommand(command) == nullptr) {
        return nullptr;
    }
    return this->GetNextArgument();
}

bool SpeedScript::IsAnotherArgument() {
    SpeedScriptEntry *entry = this->GetNextEntry();
    if (entry != nullptr && !entry->IsCommand) {
        return 1;
    }
    return 0;
}

char *SpeedScript::GetNextArgument() {
    SpeedScriptEntry *entry = this->GetNextEntry();

    if (entry && !entry->IsCommand) {
        this->NextEntryNum++;
        return this->GetName(entry);
    }
    return nullptr;
}

char *SpeedScript::PeekNextArgument() {
    const int saved_position = this->NextEntryNum;
    char *argument = this->GetNextArgument();
    this->NextEntryNum = saved_position;
    return argument;
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
    if (a < -32768 || a > 65535) {
        this->Error("Parameter %d wouldn\'t fit into short in %s\n", a, this->GetPositionName());
    }
    return a;
}

char SpeedScript::GetNextArgumentChar() {
    int a = this->GetNextArgumentInt();
    if (a < -128 || a > 255) {
        this->Error("Parameter %d wouldn\'t fit into char in %s\n", a, this->GetPositionName());
    }
    return a;
}

float SpeedScript::GetNextArgumentFloat() {
    char *s = this->GetNextArgumentString();
    float value = bStrToFloat(s);
    return value;
}

bVector2 SpeedScript::GetNextArgumentVector2() {
    float x = this->GetNextArgumentFloat();
    float y = this->GetNextArgumentFloat();
    return bVector2(x, y);
}

bVector3 SpeedScript::GetNextArgumentVector3() {
    float x = this->GetNextArgumentFloat();
    float y = this->GetNextArgumentFloat();
    float z = this->GetNextArgumentFloat();
    return bVector3(x, y, z);
}

bVector4 SpeedScript::GetNextArgumentVector4() {
    float x = this->GetNextArgumentFloat();
    float y = this->GetNextArgumentFloat();
    float z = this->GetNextArgumentFloat();
    float w = this->GetNextArgumentFloat();

    return bVector4(x, y, z, w);
}
