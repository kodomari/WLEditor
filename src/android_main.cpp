#ifdef ANDROID_BUILD

#include <jni.h>
#include <android/log.h>
#include <string>
#include <memory>

#define LOG_TAG "WLEditor"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Simple text editor implementation for Android
class AndroidTextEditor {
private:
    std::string content;
    int cursor_pos;
    
public:
    AndroidTextEditor() : cursor_pos(0) {
        LOGI("AndroidTextEditor created");
    }
    
    void insertText(const std::string& text) {
        content.insert(cursor_pos, text);
        cursor_pos += text.length();
        LOGI("Text inserted: %s", text.c_str());
    }
    
    void deleteChar() {
        if (cursor_pos > 0) {
            content.erase(cursor_pos - 1, 1);
            cursor_pos--;
            LOGI("Character deleted");
        }
    }
    
    std::string getText() const {
        return content;
    }
    
    void setCursor(int pos) {
        if (pos >= 0 && pos <= static_cast<int>(content.length())) {
            cursor_pos = pos;
        }
    }
    
    int getCursor() const {
        return cursor_pos;
    }
};

static std::unique_ptr<AndroidTextEditor> editor;

extern "C" {

JNIEXPORT void JNICALL
Java_com_wleditor_app_MainActivity_nativeInit(JNIEnv *env, jclass clazz) {
    LOGI("WLEditor Android Native Init");
    editor = std::make_unique<AndroidTextEditor>();
    LOGI("WLEditor initialized successfully");
}

JNIEXPORT void JNICALL
Java_com_wleditor_app_MainActivity_insertText(JNIEnv *env, jclass clazz, jstring text) {
    if (editor) {
        const char* nativeText = env->GetStringUTFChars(text, 0);
        editor->insertText(std::string(nativeText));
        env->ReleaseStringUTFChars(text, nativeText);
    }
}

JNIEXPORT jstring JNICALL
Java_com_wleditor_app_MainActivity_getText(JNIEnv *env, jclass clazz) {
    if (editor) {
        return env->NewStringUTF(editor->getText().c_str());
    }
    return env->NewStringUTF("");
}

JNIEXPORT void JNICALL
Java_com_wleditor_app_MainActivity_deleteChar(JNIEnv *env, jclass clazz) {
    if (editor) {
        editor->deleteChar();
    }
}

// JNI_OnLoad is called when the library is loaded
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("WLEditor Native Library Loaded");
    return JNI_VERSION_1_6;
}

} // extern "C"

#endif // ANDROID_BUILD