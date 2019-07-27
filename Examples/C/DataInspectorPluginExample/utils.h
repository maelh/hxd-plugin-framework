#pragma once
        
#define ARRAY_LENGTH(x)  (sizeof(x) / sizeof((x)[0]))
#define FIELD_SIZEOF(type, field)  (sizeof(((type *)0)->field))
