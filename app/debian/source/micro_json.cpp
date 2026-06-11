/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
 *  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
 *
 *  This program is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,   but WITHOUT
 *  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
 *
 *  You should have received a copy of the  GNU General Public License along with
 *  this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ==============================================================================
 */

#include "includes.h"

bool MicroJson::is_part_of_number(char charAt) {
    return (((charAt >= '0') && (charAt <= '9')) ||
            ((charAt >= 'a') && (charAt <= 'f')) ||
            ((charAt >= 'A') && (charAt <= 'F')) ||
            (charAt == 'x') || (charAt == '-') || (charAt == '+') || 
            (charAt == '.') || (charAt == ','));
}

std::string* MicroJson::bin2str(uint8_t* bin, size_t size) {
    std::string* s = new std::string();
    if ((bin != nullptr) && (size > 0)) {
        char hex[4]{ 0 };
        for (size_t i = 0; i < size; i++) {
            snprintf(hex, sizeof (hex), "%-2.2X", bin[i]);
            s->append(hex);
        }
    }
    return (s);
}

void MicroJson::str2bin(const char* cstr, uint8_t* bin, size_t size) {
    if ((cstr != nullptr) && (bin != nullptr) && (size > 0)) {
        char hex[8]{ 0 };
        hex[0] = '0';
        hex[1] = 'x';
        size_t len = strlen(cstr);
        size_t n = 0;
        for (size_t i = 0; (i < len) && (n < size); i += 2) {
            hex[2] = cstr[i + 0];
            hex[3] = cstr[i + 1];
            bin[n++] = (uint8_t)(strtoul(hex, nullptr, 16) & 0xff);
        }
    }
}

const char* MicroJson::get_chars(const char* ptr, char* target, size_t size) {
    const char* start = ptr;
    size_t quoteCount = 0;
    while (*ptr != 0) {
        if (*ptr == '\"') {
            quoteCount++;
            if (quoteCount == 1) {
                start = &ptr[1];
            } else if (quoteCount == 2) {
                size_t strLen;
                for (strLen = 0; (strLen < (size - 1)) && (start != ptr); strLen++) {
                    target[strLen] = *start++;
                }
                target[strLen] = 0;
                break;
            }
        }
        ptr++;
    }
    return (ptr);
}

const char* MicroJson::get_string(const char* ptr, std::string* str) {
    const char* start = ptr;
    size_t len = 0;
    size_t quoteCount = 0;
    while (*ptr != 0) {
        if (*ptr == '\"') {
            quoteCount++;
            if (quoteCount == 1) {
                start = &ptr[1];
            } else if (quoteCount == 2) {
                break;
            }
        } else {
            len++;
        }
        ptr++;
    }
    str->assign(start, len);
    return (ptr);
}

MicroJsonObject* MicroJson::branch(MicroJsonObject* object, std::string key, size_t depth) {
    if (object == nullptr) {
        return (nullptr);
    }
    if (key.length() < 1) {
        return (object);
    }
    const MicroJsonStruct* json_struct = object->_get_struct_();
    const size_t len = object->_get_struct_count_();
    const char* key_string = key.c_str();
    for (size_t i = 0; i < len; i++) {
        if (!strcmp(json_struct[i]._name_, key_string)) {
            const void* itemAddress = &((uint8_t*)object)[json_struct[i]._index_];
            if (json_struct[i]._type_ == MicroJsonObjectType::obj_object) {
                return (*((MicroJsonObject**)itemAddress));
            } else if (json_struct[i]._type_ == MicroJsonObjectType::obj_vector) {
                std::vector<MicroJsonObject*>* v = *((std::vector<MicroJsonObject*>**)itemAddress);
                if (v->size() > 0) {
                    MicroJsonObject* obj;
                    if ((size_t)depth < v->size()) {
                        obj = v->at(depth);
                    } else {
                        obj = (v->at(0))->_new_instance_();
                        v->push_back(obj);
                    }
                    return (obj);
                }
                break;
            } else {
                break;
            }
        }
    }
    return (object);
}

void MicroJson::check_locale(std::string& value) {
    std::replace(value.begin(), value.end(), ',', '.');
}

void MicroJson::parse_item(MicroJsonObject* object, std::string root, size_t depth, std::string key, std::string value) {
    const MicroJsonStruct* json_struct = object->_get_struct_();
    const size_t len = object->_get_struct_count_();
    for (size_t i = 0; i < len; i++) {
        int result;
        const char* key_name;
        if (json_struct[i]._type_ == MicroJsonObjectType::obj_vector) {
            key_name = root.c_str();
        } else {
            key_name = key.c_str();
        }
        result = strcmp(json_struct[i]._name_, key_name);
        if (!result) {
            void* itemAddress = &((uint8_t*)object)[json_struct[i]._index_];
            switch (json_struct[i]._type_) {
                case MicroJsonObjectType::obj_chars: {
                    strncpy((char*)itemAddress, value.c_str(), json_struct[i]._size_);
                } break;

                case MicroJsonObjectType::obj_string: {
                    std::string* s = *((std::string**)itemAddress);
                    s->assign(value);
                } break;

                case MicroJsonObjectType::obj_byte: {
                    *((uint8_t*)itemAddress) = (uint8_t)(strtoul(value.c_str(), nullptr, 0) & 0xff);
                } break;

                case MicroJsonObjectType::obj_int: {
                    *((int*)itemAddress) = (int)strtoul(value.c_str(), nullptr, 0);
                } break;

                case MicroJsonObjectType::obj_short: {
                    *((int16_t*)itemAddress) = (int16_t)(strtoul(value.c_str(), nullptr, 0) & 0xffff);
                } break;

                case MicroJsonObjectType::obj_word: {
                    *((uint16_t*)itemAddress) = (uint16_t)(strtoul(value.c_str(), nullptr, 0) & 0xffff);
                } break;

                case MicroJsonObjectType::obj_long: {
                    *((int32_t*)itemAddress) = (int32_t)(strtoul(value.c_str(), nullptr, 0));
                } break;

                case MicroJsonObjectType::obj_float: {
                    check_locale(value);
                    *((float*)itemAddress) = (float)(atof(value.c_str()));
                } break;

                case MicroJsonObjectType::obj_size_t: {
                    *((uint64_t*)itemAddress) = (uint32_t)(strtouq(value.c_str(), nullptr, 0));
                } break;

                case MicroJsonObjectType::obj_binary: {
                    str2bin(value.c_str(), (uint8_t*)itemAddress, json_struct[i]._size_);
                } break;

                case MicroJsonObjectType::obj_vector:
                case MicroJsonObjectType::obj_object:
                default: {
                } break;
            } break;
        }
    }
}

void MicroJson::parse_object(MicroJsonObject* object, std::string root, size_t depth, std::string scope) {
    MicroJsonProcessingMode mode = MicroJsonProcessingMode::key;
    std::string key;
    std::string value;
    bool flag = false;

    for (size_t i = 0; i < scope.length(); i++) {
        char charAt = scope.at(i);
        if (charAt == '\"') {
            flag = !flag;
        } else {
            if (flag) {
                if (mode == MicroJsonProcessingMode::key) {
                    key.append(&charAt, 1);
                } else {
                    value.append(&charAt, 1);
                }
            } else {
                if (charAt == ':') {
                    mode = MicroJsonProcessingMode::val;
                } else if (charAt == ',') {
                    parse_item(object, root, depth, key, value);
                    key.clear();
                    value.clear();
                    mode = MicroJsonProcessingMode::key;
                } else if (is_part_of_number(charAt)) {
                    value.append(&charAt, 1);
                }
            }
        }
    }

    parse_item(object, root, depth, key, value);
}

void MicroJson::process_branch(std::string response, std::string sub_key, int& depth, MicroJsonObject* object) {
    if ((object == nullptr) || (response.length() < 1)) {
        return;
    }

    MicroJsonProcessingMode mode = MicroJsonProcessingMode::key;
    std::string scope;
    std::string sub;
    std::string key = sub_key;
    int level = 0;
    bool flag = false;
    size_t key_pos = 0;

    for (size_t i = 0; i < response.length(); i++) {
        char charAt = response.at(i);
        
        if (charAt == '{') {
            level++;
            if (level == 1) {
                continue;
            }
        }

        if (charAt == '}') {
            level--;
            if (level == 0) {
                MicroJson::process_branch(sub, key, depth, branch(object, key, depth));
                sub.clear();
                if (key.length() > 0) {
                    scope = scope.substr(0, key_pos);
                }
                mode = MicroJsonProcessingMode::key;
                continue;
            }
        }
        
        if (level == 0) {
            if (charAt == '[') {
                depth = 0;
                continue;
            }

            if (charAt == ']') {
                depth = 0;
                continue;
            }

            if (charAt == '\"') {
                flag = !flag;
                if (flag && (mode == MicroJsonProcessingMode::key)) {
                    key.clear();
                    size_t s = scope.length();
                    key_pos = (s > 0) ? s - 1 : s;
                }
            } else {
                if (flag) {
                    if (mode == MicroJsonProcessingMode::key) {
                        key.append(&charAt, 1);
                    }
                } else {
                    if (charAt == ':') {
                        mode = MicroJsonProcessingMode::val;
                    } else {
                        if (charAt == ',') {
                            mode = MicroJsonProcessingMode::key;
                        }
                    }
                }
            }

            if (flag || (charAt != ' ')) {
                scope.append(&charAt, 1);
            }
        } else {
            if (charAt == '\"') {
                flag = !flag;
            }

            if (flag || (charAt != ' ')) {
                sub.append(&charAt, 1);
            }
        }
    }

    if (scope.length() > 3) {
        parse_object(object, sub_key, depth++, scope);
    }
}

std::string MicroJson::encode(MicroJsonObject* object) {
    std::string jsonString;
    jsonString.append("{");

    const MicroJsonStruct* MicroJsonStruct = object->_get_struct_();
    if (object != nullptr) {

        size_t objectItemCount = object->_get_struct_count_();
        for (size_t i = 0; i < objectItemCount; i++) {

            char buffer[1024]{ 0 };
            void* itemAddress = &((uint8_t*)object)[MicroJsonStruct[i]._index_];

            if (i > 0) {
                jsonString.append(",");
            }

            switch (MicroJsonStruct[i]._type_) {
                case MicroJsonObjectType::obj_binary: {
                    uint8_t* bin = (uint8_t*)itemAddress;
                    size_t size = MicroJsonStruct[i]._size_;
                    std::string* s = bin2str(bin, size);
                    snprintf(buffer, sizeof (buffer), "\"%s\":\"%s\"", MicroJsonStruct[i]._name_, s->c_str());
                    delete (s);
                } break;

                case MicroJsonObjectType::obj_string: {
                    std::string* s = *((std::string**)itemAddress);
                    snprintf(buffer, sizeof (buffer), "\"%s\":\"%s\"", MicroJsonStruct[i]._name_, s->c_str());
                } break;

                case MicroJsonObjectType::obj_int: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%d", MicroJsonStruct[i]._name_, *((int*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_byte: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%u", MicroJsonStruct[i]._name_, *((uint8_t*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_short: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%d", MicroJsonStruct[i]._name_, *((int16_t*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_word: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%u", MicroJsonStruct[i]._name_, *((uint16_t*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_long: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%ld", MicroJsonStruct[i]._name_, (long)*((int32_t*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_float: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%g", MicroJsonStruct[i]._name_, (double)*((double*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_size_t: {
                    snprintf(buffer, sizeof (buffer), "\"%s\":%llu", MicroJsonStruct[i]._name_, (unsigned long long int)*((uint64_t*)itemAddress));
                } break;

                case MicroJsonObjectType::obj_vector: {
                    jsonString.append("[");
                    std::vector<MicroJsonObject*>* vect = *((std::vector<MicroJsonObject*>**)itemAddress);
                    if (vect != nullptr) {
                        for (size_t index = 0; index < vect->size(); index++) {
                            MicroJsonObject* subObject = vect->at(index);
                            jsonString.append(encode(subObject));
                        }
                    }
                    jsonString.append("]");
                } break;

                case MicroJsonObjectType::obj_object: {
                    jsonString.append(encode(*((MicroJsonObject**)itemAddress)));
                } break;

                default: {
                } break;
            }

            jsonString.append(buffer);
        }
    }

    jsonString.append("}");

    return (jsonString);
}

bool MicroJson::parse(std::string string, MicroJsonObject* object) {
    char* old_locale = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");

    std::string key;
    int depth = 0;
    process_branch(string, key, depth, object);

    setlocale(LC_NUMERIC, old_locale);
    return (true);
}
