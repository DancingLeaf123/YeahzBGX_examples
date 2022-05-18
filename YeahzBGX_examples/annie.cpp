#include "../plugin_sdk/plugin_sdk.hpp"
#include "annie.h"

namespace annie
{
	// Define the colors that will be used in on_draw()

#define Q_DRAW_COLOR (MAKE_COLOR ( 62, 129, 237, 255 ))  //Red Green Blue Alpha
#define W_DRAW_COLOR (MAKE_COLOR ( 227, 203, 20, 255 ))  //Red Green Blue Alpha
#define E_DRAW_COLOR (MAKE_COLOR ( 235, 12, 223, 255 ))  //Red Green Blue Alpha
#define R_DRAW_COLOR (MAKE_COLOR ( 224, 77, 13, 255 ))   //Red Green Blue Alpha


// To declare a spell, it is necessary to create an object and registering it in load function
    script_spell* q = nullptr;
    script_spell* w = nullptr;
    script_spell* e = nullptr;
    script_spell* r = nullptr;

    script_spell* flash = nullptr;

    // Declaration of menu objects
    TreeTab* main_tab = nullptr;

    namespace draw_settings
    {
        TreeEntry* draw_range_q = nullptr;
        TreeEntry* draw_range_w = nullptr;
        TreeEntry* draw_range_e = nullptr;
        TreeEntry* draw_range_r = nullptr;
        TreeEntry* draw_flash_range = nullptr;
    }


    namespace combo
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_r = nullptr;
    }

    namespace harass
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
    }

    namespace laneclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
    }

    namespace jungleclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* auto_disable_e = nullptr;
    }

    namespace fleemode
    {
        TreeEntry* stack_w;
        TreeEntry* use_e;
    }

    // Event handler functions
    void on_update();
    void on_draw();
    void on_before_attack(game_object_script target, bool* process);

    // Declaring functions responsible for spell-logic
    //
    void q_logic();
    void w_logic();
    void e_logic();
    void r_logic();

    // Enum is used to define myhero region 
    enum Position
    {
        Line,
        Jungle
    };

    Position my_hero_region;


    void load()
    {
        // Registering a spells
        //
        q = plugin_sdk->register_spell(spellslot::q, 625);
        w = plugin_sdk->register_spell(spellslot::w, 590);
        e = plugin_sdk->register_spell(spellslot::e, 0);
        r = plugin_sdk->register_spell(spellslot::r, 600);








    }

















}

