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

#pragma once

#define JSON_GETTERS(class_name, def_array_name) \
    MicroJsonObject* _new_instance_(void) override { return (new class_name ()); } \
    const MicroJsonStruct* _get_struct_(void) override { return (def_array_name); } \
    size_t _get_struct_count_(void) override { return (sizeof (def_array_name) / sizeof (def_array_name[0])); } \
    const char* _get_name_(void) override { return (#class_name); }

#define JSON_ITEM(className, itemName, itemType) { #itemName, (size_t)(&((className*)nullptr)->itemName), sizeof (className::itemName), itemType }

#define JSON_VECTOR(className, itemName) { \
                itemName = new std::vector<className*>(); \
                itemName->push_back(new className()); \
            }

#define JSON_VECTOR_DELETE(className, itemName) { \
                for (size_t i = 0; i < itemName->size(); i++) { \
                    className* object = itemName->at(i); \
                    if (object != nullptr) { \
                        delete (object); \
                    } \
                } \
                delete (itemName); \
            }

enum class MicroJsonProcessingMode {
    key = 0,
    val = 1,
};

enum class MicroJsonObjectType {
    obj_chars,
    obj_string,
    obj_byte,
    obj_int,
    obj_short,
    obj_word,
    obj_long,
    obj_float,
    obj_size_t,
    obj_binary,
    obj_vector,
    obj_object,
};

class MicroJsonStruct {
    public:
        const char* _name_;
        size_t _index_;
        size_t _size_;
        MicroJsonObjectType _type_;
};

class MicroJsonObject {
    public:
        virtual ~MicroJsonObject() {};
        virtual MicroJsonObject* _new_instance_(void) = 0;
        virtual const MicroJsonStruct* _get_struct_(void) = 0;
        virtual size_t _get_struct_count_(void) = 0;
        virtual const char* _get_name_(void) = 0;
};

class MicroJson {
    private:
        static void str2bin(const char* cstr, uint8_t* bin, size_t size);
        static const char* get_chars(const char* p, char* target, size_t size);
        static const char* get_string(const char* p, std::string* s);
        static MicroJsonObject* branch(MicroJsonObject* object, std::string key, size_t depth);
        static void process_branch(std::string scope, std::string sub_key, int& depth, MicroJsonObject* object);
        static void parse_object(MicroJsonObject* object, std::string root_key, size_t depth, std::string scope);
        static void parse_item(MicroJsonObject* object, std::string root_key, size_t depth, std::string key, std::string value);
        static bool is_part_of_number(char charAt);
        static std::string* bin2str(uint8_t* bin, size_t size);
        static void check_locale(std::string& value);

    public:
        static bool parse(std::string string, MicroJsonObject* object);
        static std::string encode(MicroJsonObject* object);
};

