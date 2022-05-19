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
		TreeEntry* e_myhero_hp_under = nullptr;
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
		TreeEntry* stack_e;
	}

	namespace misc
	{
		TreeEntry* use_e = nullptr;
		TreeEntry* time = nullptr;
		TreeEntry* over_my_hp_in_percent = nullptr;
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

		// Checking which slot the Summoner Flash is on 
		//
		if (myhero->get_spell(spellslot::summoner1)->get_spell_data()->get_name_hash() == spell_hash("SummonerFlash"))
			flash = plugin_sdk->register_spell(spellslot::summoner1, 400.f);
		else if (myhero->get_spell(spellslot::summoner2)->get_spell_data()->get_name_hash() == spell_hash("SummonerFlash"))
			flash = plugin_sdk->register_spell(spellslot::summoner2, 400.f);


		main_tab = menu->create_tab("Yeahz_Annie", "Yeahz Annie test");
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
		{
			auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
			{
				combo::use_q = combo->add_checkbox(myhero->get_model() + ".comboUseQ", "Use Q", true);
				combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				combo::use_w = combo->add_checkbox(myhero->get_model() + ".comboUseW", "Use W", true);
				combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
				combo::use_e = combo->add_checkbox(myhero->get_model() + ".comboUseE", "Use E", true);
				combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				auto e_config = combo->add_tab(myhero->get_model() + ".comboWConfig", "E Config");
				{
					combo::e_myhero_hp_under = e_config->add_slider(myhero->get_model() + ".comboMyheroHpUnder", "Myhero HP is under (in %)", 60, 0, 100);
				}
				combo::use_r = combo->add_checkbox(myhero->get_model() + ".comboUseR", "Use R", true);
				combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
			}

			auto misc = main_tab->add_tab(myhero->get_model() + ".misc", "Misc Settings");
			{
				misc::use_e = misc->add_checkbox(myhero->get_model() + ".miscUseW", "Use E", true);
				misc::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				auto e_config = misc->add_tab(myhero->get_model() + ".miscWConfig", "E Config");
				{
					misc::time = e_config->add_slider(myhero->get_model() + ".miscWConfigTime", "Set coming damage time (in ms)", 150, 0, 1000);
					misc::over_my_hp_in_percent = e_config->add_slider(myhero->get_model() + ".miscWConfigOverMyHpInPercent", "Coming damage is over my HP (in %)", 15, 0, 100);
				}
			}










		}

		// To add a new event you need to define a function and call add_calback
		//
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_draw);
		event_handler<events::on_before_attack_orbwalker>::add_callback(on_before_attack);
	}

	void unload()
	{
		// Always remove all declared spells
		//
		plugin_sdk->remove_spell(q);
		plugin_sdk->remove_spell(w);
		plugin_sdk->remove_spell(e);
		plugin_sdk->remove_spell(r);

		if (flash)
			plugin_sdk->remove_spell(flash);


		// VERY important to remove always ALL events
		//
		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_draw>::remove_handler(on_draw);
		event_handler<events::on_before_attack_orbwalker>::remove_handler(on_before_attack);
	}

	void on_update() {
		if (myhero->is_dead())
		{
			return;
		}

		if (!myhero->has_buff(buff_hash("AnnieE")) && misc::use_e->get_bool() && e->is_ready())
		{
			if ((health_prediction->get_incoming_damage(myhero, misc::time->get_int() / 1000.f, true) * 100.f) /
				myhero->get_max_health() > myhero->get_health_percent() * (misc::over_my_hp_in_percent->get_int() / 100.f))
			{
				w->cast();
			}
		}


		if (orbwalker->can_move(0.05f))
		{
			//Checking if the user has combo_mode() (Default SPACE)
			if (orbwalker->combo_mode())
			{
				if (r->is_ready() && combo::use_r->get_bool())
				{
					r_logic();
				}

				if (q->is_ready() && combo::use_q->get_bool())
				{
					q_logic();
				}

				if (w->is_ready() && combo::use_w->get_bool())
				{
					w_logic();
				}

				if (e->is_ready() && combo::use_e->get_bool())
				{
					e_logic();
				}
			}
		}
	}


#pragma region q_logic
	void q_logic()
	{
		// Get a target from a given range
		auto target = target_selector->get_target(625, damage_type::magical);

		// Always check an object is not a nullptr!
		if (target != nullptr)
		{
			// Check if the distance between myhero and enemy is smaller than q range
			if (target->get_distance(myhero) <= q->range())
			{
				q->cast()
					return;
			}
		}
	}
#pragma endregion









}

