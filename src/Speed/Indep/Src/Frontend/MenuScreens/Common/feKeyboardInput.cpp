extern void WideStringToPackedString(char *dest, int destSize, const unsigned short *src);

void KeyboardEditString::SyncEditIntoPacked() {
    WideStringToPackedString(EditStringPacked, 0x100, EditStringUCS2);
}

char *KeyboardEditString::GetEditedString() {
    SyncEditIntoPacked();
    return EditStringPacked;
}
