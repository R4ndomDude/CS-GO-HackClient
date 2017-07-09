#include "sdk.h"

void CMenu::Setup()
{
	int iScrW, iScrH, iMenuW = 600, iMenuH = 400;
	I::Engine->GetScreenSize(iScrW, iScrH);

	frame = new MenuElements::Frame;
	frame->SetPosition(iScrW / 2 - iMenuW / 2, iScrH / 2 - iMenuH / 2);
	frame->SetSize(iMenuW, iMenuH);

	auto aimbot_tab = new MenuElements::Tab;
	aimbot_tab->SetTab(0);
	aimbot_tab->SetTitle("Aimbot");

	static int checkbox = 0;

	auto aimbot_enabled = new MenuElements::Checkbox;
	aimbot_enabled->SetPosition(94, 0 + (18 * checkbox));
	aimbot_enabled->SetTitle("Enabled");
	aimbot_enabled->SetValue(Config->GetValuePtr("Aimbot", "Enabled"));
	aimbot_tab->Add(aimbot_enabled);

	auto aimbot_usekey = new MenuElements::Checkbox;
	aimbot_usekey->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_usekey->SetTitle("Use Key");
	aimbot_usekey->SetValue(Config->GetValuePtr("Aimbot", "Use Key"));
	aimbot_tab->Add(aimbot_usekey);

	auto aimbot_smoothing = new MenuElements::Checkbox;
	aimbot_smoothing->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_smoothing->SetTitle("Smoothing");
	aimbot_smoothing->SetValue(Config->GetValuePtr("Aimbot", "Smoothing"));
	aimbot_tab->Add(aimbot_smoothing);

	auto aimbot_rcs = new MenuElements::Checkbox;
	aimbot_rcs->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_rcs->SetTitle("Recoil Control");
	aimbot_rcs->SetValue(Config->GetValuePtr("Aimbot", "RCS"));
	aimbot_tab->Add(aimbot_rcs);

	auto aimbot_silent = new MenuElements::Checkbox;
	aimbot_silent->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_silent->SetTitle("Silent");
	aimbot_silent->SetValue(Config->GetValuePtr("Aimbot", "Silent"));
	aimbot_tab->Add(aimbot_silent);

	auto aimbot_spinbot = new MenuElements::Checkbox;
	aimbot_spinbot->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_spinbot->SetTitle("Spinbot");
	aimbot_spinbot->SetValue(Config->GetValuePtr("Aimbot", "Spinbot"));
	aimbot_tab->Add(aimbot_spinbot);

	auto aimbot_aimstep = new MenuElements::Checkbox;
	aimbot_aimstep->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_aimstep->SetTitle("Aim Step");
	aimbot_aimstep->SetValue(Config->GetValuePtr("Aimbot", "Aim Step"));
	aimbot_tab->Add(aimbot_aimstep);

	auto aimbot_autowall = new MenuElements::Checkbox;
	aimbot_autowall->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_autowall->SetTitle("Autowall");
	aimbot_autowall->SetValue(Config->GetValuePtr("Aimbot", "Autowall"));
	aimbot_tab->Add(aimbot_autowall);

	auto aimbot_fovcheck = new MenuElements::Checkbox;
	aimbot_fovcheck->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_fovcheck->SetTitle("FOV Check");
	aimbot_fovcheck->SetValue(Config->GetValuePtr("Aimbot", "FOV Check"));
	aimbot_tab->Add(aimbot_fovcheck);

	auto aimbot_hitchance = new MenuElements::Checkbox;
	aimbot_hitchance->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_hitchance->SetTitle("Hit Chance");
	aimbot_hitchance->SetValue(Config->GetValuePtr("Aimbot", "Hit Chance"));
	aimbot_tab->Add(aimbot_hitchance);

	auto aimbot_autoshoot = new MenuElements::Checkbox;
	aimbot_autoshoot->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_autoshoot->SetTitle("Auto Shoot");
	aimbot_autoshoot->SetValue(Config->GetValuePtr("Aimbot", "Auto Shoot"));
	aimbot_tab->Add(aimbot_autoshoot);

	auto aimbot_autostop = new MenuElements::Checkbox;
	aimbot_autostop->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_autostop->SetTitle("Auto Stop");
	aimbot_autostop->SetValue(Config->GetValuePtr("Aimbot", "Auto Stop"));
	aimbot_tab->Add(aimbot_autostop);

	auto aimbot_autocrouch = new MenuElements::Checkbox;
	aimbot_autocrouch->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_autocrouch->SetTitle("Auto Crouch");
	aimbot_autocrouch->SetValue(Config->GetValuePtr("Aimbot", "Auto Crouch"));
	aimbot_tab->Add(aimbot_autocrouch);

	auto aimbot_ignoreteam = new MenuElements::Checkbox;
	aimbot_ignoreteam->SetPosition(94, 0 + (18 * ++checkbox));
	aimbot_ignoreteam->SetTitle("Ignore Team");
	aimbot_ignoreteam->SetValue(Config->GetValuePtr("Aimbot", "Ignore Team"));
	aimbot_tab->Add(aimbot_ignoreteam);

	static int sliderpos = 6;// 242;

	auto aimbot_smoothness = new MenuElements::Slider;
	aimbot_smoothness->SetPosition(94 + 140, sliderpos + (28 * 0));
	aimbot_smoothness->SetSize(388 - 140 + 100, 10);
	aimbot_smoothness->SetMin(1);
	aimbot_smoothness->SetMax(30);
	aimbot_smoothness->SetTitle("Smoothness");
	aimbot_smoothness->SetValue(Config->GetValuePtr("Aimbot", "Smoothness"));
	aimbot_tab->Add(aimbot_smoothness);

	auto aimbot_rcsamount = new MenuElements::Slider;
	aimbot_rcsamount->SetPosition(94 + 140, sliderpos + (28 * 1));
	aimbot_rcsamount->SetSize(388 - 140 + 100, 10);
	aimbot_rcsamount->SetMin(0);
	aimbot_rcsamount->SetMax(100);
	aimbot_rcsamount->SetTitle("RCS Amount");
	aimbot_rcsamount->SetValue(Config->GetValuePtr("Aimbot", "RCS Amount"));
	aimbot_tab->Add(aimbot_rcsamount);

	auto aimbot_fovamount = new MenuElements::Slider;
	aimbot_fovamount->SetPosition(94 + 140, sliderpos + (28 * 2));
	aimbot_fovamount->SetSize(388 - 140 + 100, 10);
	aimbot_fovamount->SetMin(1);
	aimbot_fovamount->SetMax(90);
	aimbot_fovamount->SetTitle("FOV Amount");
	aimbot_fovamount->SetValue(Config->GetValuePtr("Aimbot", "FOV"));
	aimbot_tab->Add(aimbot_fovamount);

	auto aimbot_hitchanceamt = new MenuElements::Slider;
	aimbot_hitchanceamt->SetPosition(94 + 140, sliderpos + (28 * 3));
	aimbot_hitchanceamt->SetSize(388 - 140 + 100, 10);
	aimbot_hitchanceamt->SetMin(1);
	aimbot_hitchanceamt->SetMax(100);
	aimbot_hitchanceamt->SetTitle("Hit Chance Amount");
	aimbot_hitchanceamt->SetValue(Config->GetValuePtr("Aimbot", "Hit Chance Amount"));
	aimbot_tab->Add(aimbot_hitchanceamt);

	auto aimbot_key = new MenuElements::Key;
	aimbot_key->SetPosition(94, 0 + (18 * 14));
	aimbot_key->SetSize(80, 16);
	aimbot_key->SetTitle("Key");
	aimbot_key->SetValue(Config->GetValuePtr("Aimbot", "Key"));
	aimbot_tab->Add(aimbot_key);

	frame->Add(aimbot_tab);

	auto visuals_tab = new MenuElements::Tab;
	visuals_tab->SetTab(1);
	visuals_tab->SetTitle("Visuals");

	checkbox = 0;

	auto visuals_enabled = new MenuElements::Checkbox;
	visuals_enabled->SetPosition(94, 0 + (18 * checkbox));
	visuals_enabled->SetTitle("Enabled");
	visuals_enabled->SetValue(Config->GetValuePtr("Visuals", "Enabled"));
	visuals_tab->Add(visuals_enabled);

	auto visuals_vischeck = new MenuElements::Checkbox;
	visuals_vischeck->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_vischeck->SetTitle("Visibility Check");
	visuals_vischeck->SetValue(Config->GetValuePtr("Visuals", "Visibility Check"));
	visuals_tab->Add(visuals_vischeck);

	auto visuals_name = new MenuElements::Checkbox;
	visuals_name->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_name->SetTitle("Name");
	visuals_name->SetValue(Config->GetValuePtr("Visuals", "Name"));
	visuals_tab->Add(visuals_name);

	auto visuals_box = new MenuElements::Checkbox;
	visuals_box->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_box->SetTitle("Box");
	visuals_box->SetValue(Config->GetValuePtr("Visuals", "Box"));
	visuals_tab->Add(visuals_box);

	auto visuals_bonebox = new MenuElements::Checkbox;
	visuals_bonebox->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_bonebox->SetTitle("Bone Box");
	visuals_bonebox->SetValue(Config->GetValuePtr("Visuals", "Bone Box"));
	visuals_tab->Add(visuals_bonebox);

	auto visuals_snapline = new MenuElements::Checkbox;
	visuals_snapline->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_snapline->SetTitle("Line");
	visuals_snapline->SetValue(Config->GetValuePtr("Visuals", "Line"));
	visuals_tab->Add(visuals_snapline);

	auto visuals_healthbar = new MenuElements::Checkbox;
	visuals_healthbar->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_healthbar->SetTitle("Health Bar");
	visuals_healthbar->SetValue(Config->GetValuePtr("Visuals", "Health Bar"));
	visuals_tab->Add(visuals_healthbar);

	auto visuals_healthtext = new MenuElements::Checkbox;
	visuals_healthtext->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_healthtext->SetTitle("Health Text");
	visuals_healthtext->SetValue(Config->GetValuePtr("Visuals", "Health Text"));
	visuals_tab->Add(visuals_healthtext);

	auto visuals_chams = new MenuElements::Checkbox;
	visuals_chams->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_chams->SetTitle("Chams");
	visuals_chams->SetValue(Config->GetValuePtr("Visuals", "Chams"));
	visuals_tab->Add(visuals_chams);

	auto visuals_chamslit = new MenuElements::Checkbox;
	visuals_chamslit->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_chamslit->SetTitle("Chams Lit");
	visuals_chamslit->SetValue(Config->GetValuePtr("Visuals", "Chams Lit"));
	visuals_tab->Add(visuals_chamslit);

	auto visuals_chamswire = new MenuElements::Checkbox;
	visuals_chamswire->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_chamswire->SetTitle("Chams Wireframe");
	visuals_chamswire->SetValue(Config->GetValuePtr("Visuals", "Chams Wireframe"));
	visuals_tab->Add(visuals_chamswire);

	auto visuals_nohands = new MenuElements::Checkbox;
	visuals_nohands->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_nohands->SetTitle("No Hands");
	visuals_nohands->SetValue(Config->GetValuePtr("Visuals", "No Hands"));
	visuals_tab->Add(visuals_nohands);

	auto visuals_rainbowhands = new MenuElements::Checkbox;
	visuals_rainbowhands->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_rainbowhands->SetTitle("Rainbow Hands");
	visuals_rainbowhands->SetValue(Config->GetValuePtr("Visuals", "Rainbow Hands"));
	visuals_tab->Add(visuals_rainbowhands);

	auto visuals_enemyonly = new MenuElements::Checkbox;
	visuals_enemyonly->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_enemyonly->SetTitle("Enemy Only");
	visuals_enemyonly->SetValue(Config->GetValuePtr("Visuals", "Enemy Only"));
	visuals_tab->Add(visuals_enemyonly);

	auto visuals_recoilcross = new MenuElements::Checkbox;
	visuals_recoilcross->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_recoilcross->SetTitle("Recoil Crosshair");
	visuals_recoilcross->SetValue(Config->GetValuePtr("Visuals", "Recoil Crosshair"));
	visuals_tab->Add(visuals_recoilcross);

	auto visuals_novisrecoil = new MenuElements::Checkbox;
	visuals_novisrecoil->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_novisrecoil->SetTitle("No Visual Recoil");
	visuals_novisrecoil->SetValue(Config->GetValuePtr("Visuals", "No Visual Recoil"));
	visuals_tab->Add(visuals_novisrecoil);

	auto visuals_customfov = new MenuElements::Checkbox;
	visuals_customfov->SetPosition(94, 0 + (18 * ++checkbox));
	visuals_customfov->SetTitle("Custom FOV");
	visuals_customfov->SetValue(Config->GetValuePtr("Visuals", "Custom FOV"));
	visuals_tab->Add(visuals_customfov);

	sliderpos = 6;// 314;

	auto visuals_teamcolor = new MenuElements::ColorPicker;
	visuals_teamcolor->SetPosition(94 + 140, sliderpos + (28 * 0));
	visuals_teamcolor->SetSize(388 - 140 + 100, 10);
	visuals_teamcolor->SetTitle("Team Color");
	visuals_teamcolor->SetValue(Config->GetValuePtr("Visuals", "Team Color"));
	visuals_tab->Add(visuals_teamcolor);

	auto visuals_enemycolor = new MenuElements::ColorPicker;
	visuals_enemycolor->SetPosition(94 + 140, sliderpos + (28 * 1));
	visuals_enemycolor->SetSize(388 - 140 + 100, 10);
	visuals_enemycolor->SetTitle("Enemy Color");
	visuals_enemycolor->SetValue(Config->GetValuePtr("Visuals", "Enemy Color"));
	visuals_tab->Add(visuals_enemycolor);

	auto visuals_fovamt = new MenuElements::Slider;
	visuals_fovamt->SetPosition(94 + 140, sliderpos + (28 * 2));
	visuals_fovamt->SetSize(388 - 140 + 100, 10);
	visuals_fovamt->SetMin(1.f);
	visuals_fovamt->SetMax(179.f);
	visuals_fovamt->SetTitle("FOV Amount");
	visuals_fovamt->SetValue(Config->GetValuePtr("Visuals", "FOV Amount"));
	visuals_tab->Add(visuals_fovamt);

	frame->Add(visuals_tab);

	auto misc_tab = new MenuElements::Tab;
	misc_tab->SetTab(2);
	misc_tab->SetTitle("Misc");
	frame->Add(misc_tab);

	checkbox = 0;

	auto misc_bhop = new MenuElements::Checkbox;
	misc_bhop->SetPosition(94, 0 + (18 * checkbox));
	misc_bhop->SetTitle("Bhop");
	misc_bhop->SetValue(Config->GetValuePtr("Misc", "Bhop"));
	misc_tab->Add(misc_bhop);

	auto misc_autostrafe = new MenuElements::Checkbox;
	misc_autostrafe->SetPosition(94, 0 + (18 * ++checkbox));
	misc_autostrafe->SetTitle("Auto Strafe");
	misc_autostrafe->SetValue(Config->GetValuePtr("Misc", "Auto Strafe"));
	misc_tab->Add(misc_autostrafe);

	auto misc_namesteal = new MenuElements::Checkbox;
	misc_namesteal->SetPosition(94, 0 + (18 * ++checkbox));
	misc_namesteal->SetTitle("Name Steal");
	misc_namesteal->SetValue(Config->GetValuePtr("Misc", "Name Steal"));
	misc_tab->Add(misc_namesteal);

	auto misc_achievement = new MenuElements::Checkbox;
	misc_achievement->SetPosition(94, 0 + (18 * ++checkbox));
	misc_achievement->SetTitle("Achievement Spam");
	misc_achievement->SetValue(Config->GetValuePtr("Misc", "Achievement Spam"));
	misc_tab->Add(misc_achievement);

	auto misc_location = new MenuElements::Checkbox;
	misc_location->SetPosition(94, 0 + (18 * ++checkbox));
	misc_location->SetTitle("Location Spam");
	misc_location->SetValue(Config->GetValuePtr("Misc", "Location Spam"));
	misc_tab->Add(misc_location);

	auto misc_airstuck = new MenuElements::Checkbox;
	misc_airstuck->SetPosition(94, 0 + (18 * ++checkbox));
	misc_airstuck->SetTitle("Air Stuck");
	misc_airstuck->SetValue(Config->GetValuePtr("Misc", "Air Stuck"));
	misc_tab->Add(misc_airstuck);

	auto misc_airstuckkey = new MenuElements::Key;
	misc_airstuckkey->SetPosition(94, 0 + (18 * ++checkbox));
	misc_airstuckkey->SetSize(80, 16);
	misc_airstuckkey->SetTitle("Air Stuck Key");
	misc_airstuckkey->SetValue(Config->GetValuePtr("Misc", "Air Stuck Key"));
	misc_tab->Add(misc_airstuckkey);

	auto hvh_tab = new MenuElements::Tab;
	hvh_tab->SetTab(3);
	hvh_tab->SetTitle("HvH");
	frame->Add(hvh_tab);

	auto skins_tab = new MenuElements::Tab;
	skins_tab->SetTab(4);
	skins_tab->SetTitle("Skins");
	frame->Add(skins_tab);

	auto skins_changer = new MenuElements::SkinChanger;
	skins_changer->SetPosition(274, 0);
	skins_tab->Add(skins_changer);

	checkbox = 0;

	auto skins_enabled = new MenuElements::Checkbox;
	skins_enabled->SetPosition(94, 0 + (18 * checkbox));
	skins_enabled->SetTitle("Enabled");
	skins_enabled->SetValue(Config->GetValuePtr("Skins", "Enabled"));
	skins_tab->Add(skins_enabled);

	auto skins_stattrak = new MenuElements::Checkbox;
	skins_stattrak->SetPosition(94, 0 + (18 * ++checkbox));
	skins_stattrak->SetTitle("StatTrak");
	skins_stattrak->SetValue(Config->GetValuePtr("Skins", "StatTrak"));
	skins_tab->Add(skins_stattrak);

	auto skins_save = new MenuElements::Button;
	skins_save->SetPosition(94, 0 + (18 * ++checkbox));
	skins_save->SetSize(80, 16);
	skins_save->SetTitle("Save Skins");
	skins_save->SetCallback(MenuCallbacks::SaveSkins);
	skins_tab->Add(skins_save);

	auto skins_load = new MenuElements::Button;
	skins_load->SetPosition(94, 0 + (18 * ++checkbox));
	skins_load->SetSize(80, 16);
	skins_load->SetTitle("Load Skins");
	skins_load->SetCallback(MenuCallbacks::LoadSkins);
	skins_tab->Add(skins_load);

	auto players_tab = new MenuElements::Tab;
	players_tab->SetTab(5);
	players_tab->SetTitle("Players");
	frame->Add(players_tab);

	auto menu_tab = new MenuElements::Tab;
	menu_tab->SetTab(6);
	menu_tab->SetTitle("Menu");

	checkbox = 0;

	auto menu_key = new MenuElements::Key;
	menu_key->SetPosition(94, 0 + (18 * checkbox));
	menu_key->SetSize(80, 16);
	menu_key->SetTitle("Key");
	menu_key->SetValue(Config->GetValuePtr("Menu", "Key"));
	menu_tab->Add(menu_key);

	frame->Add(menu_tab);

	auto config_tab = new MenuElements::Tab;
	config_tab->SetTab(7);
	config_tab->SetTitle("Config");
	frame->Add(config_tab);

	Add(frame);
}

void CMenu::Paint()
{
	Input->GetClicked();

	for (MenuElements::Element* pElement : elements)
	{
		pElement->Tick();
	}

	POINT ptMouse = Input->GetMousePos();
	if (frame->GetVisible())
	{
		static int mouseSize = 25;

		static Vertex_t mouseShape[3];
		mouseShape[0].Init(Vector2D(ptMouse.x, ptMouse.y));
		mouseShape[1].Init(Vector2D(ptMouse.x + mouseSize, ptMouse.y + mouseSize / 2));
		mouseShape[2].Init(Vector2D(ptMouse.x + mouseSize / 2, ptMouse.y + mouseSize));

		Draw::DrawPolygon(3, mouseShape, Color(255, 255, 255, 255));
	}
}

void CMenu::Add(MenuElements::Element* element)
{
	elements.push_back(element);
}

MenuElements::Element* CMenu::GetFrame()
{
	return frame;
}

CMenu* Menu = new CMenu;