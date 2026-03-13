static const char* gLoadinScreenPackageName;

void SetLoadingScreenPackageName(const char* name) {
    gLoadinScreenPackageName = name;
}

const char* GetLoadingScreenPackageName() {
    return gLoadinScreenPackageName;
}