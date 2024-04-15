plugins {
    id("com.android.application") version "7.3.0"
}

android {
    compileSdkVersion(33)
    defaultConfig {
        applicationId = "com.AndroidApp"
        minSdkVersion(29)
        targetSdkVersion(33)
        versionCode = 1
        versionName = "1.0"
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        getByName("release") {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }

    namespace = "com.AndroidApp"

    flavorDimensions("cpuArch")
    productFlavors {
        create("arm7") {
            ndk.abiFilters.add("armeabi-v7a")
        }
        create("arm8") {
            ndk.abiFilters.add("arm64-v8a")
        }
        create("x86") {
            ndk.abiFilters.add("x86")
        }
        create("x86-64") {
            ndk.abiFilters.add("x86_64")
        }
        create("all")
    }
}

dependencies {
    //implementation("com.android.tools.build:gradle:7.3.0")
    implementation("androidx.appcompat:appcompat:1.3.0")
    // implementation("com.google.android.material:material:1.5.0")
    // implementation("androidx.constraintlayout:constraintlayout:2.1.0")
    // androidTestImplementation("androidx.test.ext:junit:1.1.3")
    // androidTestImplementation("androidx.test.espresso:espresso-core:3.4.0")
}

// Apply a specific Java toolchain to ease working on different environments.
java {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
    toolchain {
        languageVersion = JavaLanguageVersion.of(17)
    }
}

dependencies {
    api(fileTree(mapOf("dir" to "libs", "include" to listOf("*.jar"))))
}