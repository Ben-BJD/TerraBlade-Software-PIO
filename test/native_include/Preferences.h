// test/native_include/Preferences.h
#ifndef FAKE_PREFERENCES_H
#define FAKE_PREFERENCES_H

#include <string>
#include <map>

#if defined(NATIVE_TESTING)
#include <Arduino.h>
#endif

// Preference data types enumeration
typedef enum {
    PT_I8 = 0,
    PT_U8 = 1,
    PT_I16 = 2,
    PT_U16 = 3,
    PT_I32 = 4,
    PT_U32 = 5,
    PT_I64 = 6,
    PT_U64 = 7,
    PT_STR = 8,
    PT_BLOB = 9,
    PT_INVALID = 10
} PreferenceType;

class Preferences {
private:
    // Static storage shared across all instances - using function-local statics to avoid multiple definition errors
    static std::map<std::string, std::map<std::string, int>>& getStaticIntMap() {
        static std::map<std::string, std::map<std::string, int>> staticIntMap;
        return staticIntMap;
    }
    
    static std::map<std::string, std::map<std::string, std::string>>& getStaticStringMap() {
        static std::map<std::string, std::map<std::string, std::string>> staticStringMap;
        return staticStringMap;
    }
    
    std::string _currentNamespace;
    bool _isReadOnly = false;

public:
    Preferences() = default;
    
    ~Preferences() = default;

    // Open namespace
    bool begin(const char* name, bool readOnly = false, const char* partition_label = NULL) {
        _currentNamespace = std::string(name);
        _isReadOnly = readOnly;
        return true;
    }

    // Close namespace
    void end() {
        _currentNamespace = "";
        _isReadOnly = false;
    }

    // Clear all keys in current namespace
    bool clear() {
        if (_isReadOnly) return false;
        if (_currentNamespace.empty()) return false;
        
        getStaticIntMap()[_currentNamespace].clear();
        getStaticStringMap()[_currentNamespace].clear();
        return true;
    }

    // Remove a key
    bool remove(const char* key) {
        if (_isReadOnly) return false;
        if (_currentNamespace.empty()) return false;
        if (!key) return false;
        
        std::string keyStr(key);
        getStaticIntMap()[_currentNamespace].erase(keyStr);
        getStaticStringMap()[_currentNamespace].erase(keyStr);
        return true;
    }

    // Check if key exists
    bool isKey(const char* key) {
        if (_currentNamespace.empty()) return false;
        if (!key) return false;
        
        std::string keyStr(key);
        return getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end() ||
               getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end();
    }

    // Put methods
    size_t putChar(const char* key, int8_t value) {
        return putInt(key, static_cast<int32_t>(value));
    }
    
    size_t putUChar(const char* key, uint8_t value) {
        return putUInt(key, static_cast<uint32_t>(value));
    }
    
    size_t putShort(const char* key, int16_t value) {
        return putInt(key, static_cast<int32_t>(value));
    }
    
    size_t putUShort(const char* key, uint16_t value) {
        return putUInt(key, static_cast<uint32_t>(value));
    }
    
    size_t putInt(const char* key, int32_t value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        getStaticIntMap()[_currentNamespace][std::string(key)] = static_cast<int>(value);
        return sizeof(int32_t);
    }
    
    size_t putUInt(const char* key, uint32_t value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        getStaticIntMap()[_currentNamespace][std::string(key)] = static_cast<int>(value);
        return sizeof(uint32_t);
    }
    
    size_t putLong(const char* key, int32_t value) {
        return putInt(key, value);
    }
    
    size_t putULong(const char* key, uint32_t value) {
        return putUInt(key, value);
    }
    
    size_t putLong64(const char* key, int64_t value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        getStaticIntMap()[_currentNamespace][std::string(key)] = static_cast<int>(value);
        return sizeof(int64_t);
    }
    
    size_t putULong64(const char* key, uint64_t value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        getStaticIntMap()[_currentNamespace][std::string(key)] = static_cast<int>(value);
        return sizeof(uint64_t);
    }
    
    size_t putFloat(const char* key, float value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        // Store float as int representation
        union { float f; int i; } u;
        u.f = value;
        getStaticIntMap()[_currentNamespace][std::string(key)] = u.i;
        return sizeof(float);
    }
    
    size_t putDouble(const char* key, double value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        // Store double as int representation
        union { double d; int64_t i; } u;
        u.d = value;
        getStaticIntMap()[_currentNamespace][std::string(key)] = static_cast<int>(u.i);
        return sizeof(double);
    }
    
    size_t putBool(const char* key, bool value) {
        return putInt(key, value ? 1 : 0);
    }
    
    size_t putString(const char* key, const char* value) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key || !value) return 0;
        
        getStaticStringMap()[_currentNamespace][std::string(key)] = std::string(value);
        return strlen(value);
    }
    
    size_t putString(const char* key, String value) {
        return putString(key, value.c_str());
    }
    
    size_t putBytes(const char* key, const void* value, size_t len) {
        if (_isReadOnly) return 0;
        if (_currentNamespace.empty()) return 0;
        if (!key || !value || len == 0) return 0;
        
        // Store bytes as string representation
        const uint8_t* bytes = static_cast<const uint8_t*>(value);
        std::string byteStr(reinterpret_cast<const char*>(bytes), len);
        getStaticStringMap()[_currentNamespace][std::string(key)] = byteStr;
        return len;
    }

    // Get methods
    int8_t getChar(const char* key, int8_t defaultValue = 0) {
        return static_cast<int8_t>(getInt(key, static_cast<int32_t>(defaultValue)));
    }
    
    uint8_t getUChar(const char* key, uint8_t defaultValue = 0) {
        return static_cast<uint8_t>(getUInt(key, static_cast<uint32_t>(defaultValue)));
    }
    
    int16_t getShort(const char* key, int16_t defaultValue = 0) {
        return static_cast<int16_t>(getInt(key, static_cast<int32_t>(defaultValue)));
    }
    
    uint16_t getUShort(const char* key, uint16_t defaultValue = 0) {
        return static_cast<uint16_t>(getUInt(key, static_cast<uint32_t>(defaultValue)));
    }
    
    int32_t getInt(const char* key, int32_t defaultValue = 0) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            return static_cast<int32_t>(getStaticIntMap()[_currentNamespace][keyStr]);
        }
        return defaultValue;
    }
    
    uint32_t getUInt(const char* key, uint32_t defaultValue = 0) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            return static_cast<uint32_t>(getStaticIntMap()[_currentNamespace][keyStr]);
        }
        return defaultValue;
    }
    
    int32_t getLong(const char* key, int32_t defaultValue = 0) {
        return getInt(key, defaultValue);
    }
    
    uint32_t getULong(const char* key, uint32_t defaultValue = 0) {
        return getUInt(key, defaultValue);
    }
    
    int64_t getLong64(const char* key, int64_t defaultValue = 0) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            return static_cast<int64_t>(getStaticIntMap()[_currentNamespace][keyStr]);
        }
        return defaultValue;
    }
    
    uint64_t getULong64(const char* key, uint64_t defaultValue = 0) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            return static_cast<uint64_t>(getStaticIntMap()[_currentNamespace][keyStr]);
        }
        return defaultValue;
    }
    
    float getFloat(const char* key, float defaultValue = NAN) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            union { float f; int i; } u;
            u.i = getStaticIntMap()[_currentNamespace][keyStr];
            return u.f;
        }
        return defaultValue;
    }
    
    double getDouble(const char* key, double defaultValue = NAN) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            union { double d; int64_t i; } u;
            u.i = getStaticIntMap()[_currentNamespace][keyStr];
            return u.d;
        }
        return defaultValue;
    }
    
    bool getBool(const char* key, bool defaultValue = false) {
        return getInt(key, defaultValue ? 1 : 0) != 0;
    }
    
    size_t getString(const char* key, char* value, size_t maxLen) {
        if (_currentNamespace.empty()) return 0;
        if (!key || !value || maxLen == 0) return 0;
        
        std::string keyStr(key);
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            std::string str = getStaticStringMap()[_currentNamespace][keyStr];
            size_t len = str.length() < maxLen - 1 ? str.length() : maxLen - 1;
            strncpy(value, str.c_str(), len);
            value[len] = '\0';
            return len + 1;
        }
        value[0] = '\0';
        return 0;
    }
    
    String getString(const char* key, String defaultValue = String()) {
        if (_currentNamespace.empty()) return defaultValue;
        if (!key) return defaultValue;
        
        std::string keyStr(key);
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            return String(getStaticStringMap()[_currentNamespace][keyStr].c_str());
        }
        return defaultValue;
    }
    
    size_t getStringLength(const char* key) {
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        std::string keyStr(key);
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            return getStaticStringMap()[_currentNamespace][keyStr].length() + 1; // +1 for null terminator
        }
        return 0;
    }
    
    size_t getBytes(const char* key, void* buf, size_t maxLen) {
        if (_currentNamespace.empty()) return 0;
        if (!key || !buf || maxLen == 0) return 0;
        
        std::string keyStr(key);
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            std::string byteStr = getStaticStringMap()[_currentNamespace][keyStr];
            size_t len = byteStr.length() < maxLen ? byteStr.length() : maxLen;
            memcpy(buf, byteStr.c_str(), len);
            return len;
        }
        return 0;
    }
    
    size_t getBytesLength(const char* key) {
        if (_currentNamespace.empty()) return 0;
        if (!key) return 0;
        
        std::string keyStr(key);
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            return getStaticStringMap()[_currentNamespace][keyStr].length();
        }
        return 0;
    }

    // Get type of key
    PreferenceType getType(const char* key) {
        if (_currentNamespace.empty()) return PT_INVALID;
        if (!key) return PT_INVALID;
        
        std::string keyStr(key);
        if (getStaticIntMap()[_currentNamespace].find(keyStr) != getStaticIntMap()[_currentNamespace].end()) {
            return PT_I32; // Simplified for mock
        }
        if (getStaticStringMap()[_currentNamespace].find(keyStr) != getStaticStringMap()[_currentNamespace].end()) {
            return PT_STR; // Simplified for mock
        }
        return PT_INVALID;
    }

    // Get free entries (simplified)
    size_t freeEntries() {
        return 1000; // Arbitrary large number for testing
    }

    // Test helper methods
    void resetMock() {
        getStaticIntMap().clear();
        getStaticStringMap().clear();
        _currentNamespace = "";
        _isReadOnly = false;
    }
};

#endif // FAKE_PREFERENCES_H
