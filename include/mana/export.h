#pragma once

#ifdef MANA_EXPORTS
    #define MANA_API __declspec(dllexport)
#else
    #define MANA_API __declspec(dllimport)
#endif
