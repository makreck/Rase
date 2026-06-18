#pragma once

class ScaleJson : public MicroJsonObject {
    public:
        char     key[32]{ 0 };

        char     name[32]{ 0 };
        char     shortcut[4]{ 0 };
        char     unit[8]{ 0 };
        char     color[10]{ 0 };

        float    bottom     = 0.0f;
        float    top        = 0.0f;
        float    value      = 0.0f;
        float    min        = 0.0f;
        float    max        = 0.0f;
        float    step       = 0.0f;
        float    zoom_begin = 0.0f;
        float    zoom_end   = 0.0f;
        float    average    = 0.0f;
        
        int      count      = 0;
        uint32_t flags      = 0;
        ColorRef color_ref  = 0;

        const MicroJsonStruct meta_data[16] = {
            JSON_ITEM(ScaleJson, name,       MicroJsonObjectType::obj_chars),
            JSON_ITEM(ScaleJson, shortcut,   MicroJsonObjectType::obj_chars),
            JSON_ITEM(ScaleJson, unit,       MicroJsonObjectType::obj_chars),
            JSON_ITEM(ScaleJson, color,      MicroJsonObjectType::obj_chars),
            JSON_ITEM(ScaleJson, bottom,     MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, top,        MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, value,      MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, min,        MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, max,        MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, step,       MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, zoom_begin, MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, zoom_end,   MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, average,    MicroJsonObjectType::obj_float),
            JSON_ITEM(ScaleJson, count,      MicroJsonObjectType::obj_int),
            JSON_ITEM(ScaleJson, flags,      MicroJsonObjectType::obj_long),
            JSON_ITEM(ScaleJson, color_ref,  MicroJsonObjectType::obj_long),
        };
        JSON_GETTERS(ScaleJson, meta_data);
};
