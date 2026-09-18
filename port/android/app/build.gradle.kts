plugins { id("com.android.application") version "8.5.0" }
android {
    namespace = "dev.nfsmw.port"
    compileSdk = 35
    ndkVersion = "28.2.13676358"
    defaultConfig {
        applicationId = "dev.nfsmw.port"
        minSdk = 28; targetSdk = 35; versionCode = 1; versionName = "0.1"
        ndk { abiFilters += listOf("arm64-v8a") }
        externalNativeBuild { cmake { targets += "nfsmw_probe" } }
    }
    externalNativeBuild { cmake { path = file("src/main/cpp/CMakeLists.txt"); version = "3.22.1" } }
}
