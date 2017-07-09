#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" // for the unload meme
#include "Interfaces.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

AyyWareWindow Menu::Window;

void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");
}

void AyyWareWindow::Setup()
{
	SetPosition(50, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("Dank memes cant melt steel beams kid ~ Tomori");

	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();

#pragma region Bottom Buttons
	UnloadButton.SetText("PANIC");
	UnloadButton.SetCallback(UnLoadCallbk);
	UnloadButton.SetPosition(16, Client.bottom - 42);

	RageBotTab.RegisterControl(&UnloadButton);
	LegitBotTab.RegisterControl(&UnloadButton);
	VisualsTab.RegisterControl(&UnloadButton);
	MiscTab.RegisterControl(&UnloadButton);

#pragma endregion Setting up the settings buttons
}

void CRageBotTab::Setup()
{
	SetTitle("Rage Aimbot");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Aimbot

	AimbotGroup.SetPosition(16, 48);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(376, 280);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(39.f);
	AimbotGroup.PlaceLabledControl("FOV Range", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silent");
	AimbotGroup.PlaceLabledControl("Silent Aim", this, &AimbotSilentAim);

	AimbotPSilent.SetFileId("aim_psilent");
	AimbotGroup.PlaceLabledControl("Perfect Silent", this, &AimbotPSilent);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl("Aim Step", this, &AimbotAimStep);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key", this, &AimbotKeyBind);

	AimbotChicken.SetFileId("aim_chicken");
	AimbotGroup.PlaceLabledControl("Chikens", this, &AimbotChicken);

#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(16, 344);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(376, 134);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("Closest To Crosshair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest Health");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceLabledControl("Friendly Fire", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off"); //0
	TargetHitscan.AddItem("Head / Body"); // 1
	TargetHitscan.AddItem("All Basic"); // 2
	TargetHitscan.AddItem("All Fucking Heaps"); // 3
	TargetGroup.PlaceLabledControl("Hit Scan", this, &TargetHitscan);
#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(408, 48);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(360, 280);
	RegisterControl(&AccuracyGroup);

	AccuracySpread.SetFileId("acc_nospread");
	AccuracyGroup.PlaceLabledControl("Anti Spread", this, &AccuracySpread);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceLabledControl("Anti Recoil", this, &AccuracyRecoil);

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceLabledControl("Auto Wall", this, &AccuracyAutoWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 100.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AccuracyGroup.PlaceLabledControl("Min AWall Damage", this, &AccuracyMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AccuracyGroup.PlaceLabledControl("Auto Stop", this, &AccuracyAutoStop);

	AccuracyAutoCrouch.SetFileId("acc_crouch");
	AccuracyGroup.PlaceLabledControl("Auto Crouch", this, &AccuracyAutoCrouch);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceLabledControl("Auto Scope", this, &AccuracyAutoScope);

	AccuracySpreadLimit.SetFileId("acc_spreadlimon");
	AccuracyGroup.PlaceLabledControl("Spread Limit", this, &AccuracySpreadLimit);

	AccuracyMinimumSpread.SetFileId("acc_spreadlim");
	AccuracyMinimumSpread.SetBoundaries(0.f, 5.0f);
	AccuracyMinimumSpread.SetValue(1.5f);
	AccuracyGroup.PlaceLabledControl("Spread Limit Value", this, &AccuracyMinimumSpread);
	
	AccuracyAngleFix.SetFileId("acc_aaa");
	AccuracyGroup.PlaceLabledControl("AngleFix", this, &AccuracyAngleFix);
#pragma endregion  Accuracy controls get Setup in here

#pragma region AntiAim
	AntiAimGroup.SetPosition(408, 344);
	AntiAimGroup.SetText("Anti-Aim");
	AntiAimGroup.SetSize(360, 134);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl("Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("None");
	AntiAimPitch.AddItem("Up");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("Jitter");
	AntiAimGroup.PlaceLabledControl("X", this, &AntiAimPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("None");
	AntiAimYaw.AddItem("Spin Fast");
	AntiAimYaw.AddItem("Spin Slow");
	AntiAimYaw.AddItem("Inverse");
	AntiAimYaw.AddItem("Jitter");
	AntiAimYaw.AddItem("Flip");
	AntiAimGroup.PlaceLabledControl("Y", this, &AntiAimYaw);

#pragma endregion  AntiAim controls get setup in here
}

void CLegitBotTab::Setup()
{
	SetTitle("Legit Aimbot");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(16, 48);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 210);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFriendlyFire.SetFileId("aim_friendfire");
	AimbotGroup.PlaceLabledControl("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);
	
	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

#pragma endregion Aimbot shit

#pragma region Triggerbot
	TriggerGroup.SetPosition(272, 48);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 210);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceLabledControl("Enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceLabledControl("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);
    
	TriggerDelay.SetFileId("trig_delay");
	TriggerDelay.SetBoundaries(1, 1000);
	TriggerDelay.SetValue(50);
	TriggerGroup.PlaceLabledControl("Delay", this, &TriggerDelay);
#pragma endregion Triggerbot stuff

#pragma region Main Weapon
	WeaponMainGroup.SetPosition(16, 274);
	WeaponMainGroup.SetText("Rifles/Other");
	WeaponMainGroup.SetSize(240, 210);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.1f, 2.f); 
	WeaponMainSpeed.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl("Max Speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.1f, 30.f);
	WeaponMainFoV.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl("FOV", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainGroup.PlaceLabledControl("Recoil", this, &WeaponMainRecoil);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);
#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(272, 274);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(240, 210);
	RegisterControl(&WeaponPistGroup);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.1f, 2.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Max Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.1f, 30.f);
	WeaponPistFoV.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl("FOV", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistGroup.PlaceLabledControl("Recoil", this, &WeaponPistRecoil);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);
#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(528, 274);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(240, 210);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipSpeed.SetFileId("snip_speed");
	WeaponSnipSpeed.SetBoundaries(0.1f, 2.f);
	WeaponSnipSpeed.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl("Max Speed", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov");
	WeaponSnipFoV.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoV.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl("FOV", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("snip_recoil");
	WeaponSnipGroup.PlaceLabledControl("Recoil", this, &WeaponSnipRecoil);

	WeaponSnipHitbox.SetFileId("snip_hitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);
#pragma endregion
}

void CVisualTab::Setup()
{
	SetTitle("Visuals & Misc");

	ActiveLabel.SetPosition(16, 16);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, 16);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.SetPosition(16, 48);
	OptionsGroup.SetSize(193, 430);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl("Name", this, &OptionsName);

	OptionsHealth.SetFileId("opt_hp");
	OptionsGroup.PlaceLabledControl("Health", this, &OptionsHealth);

	OptionsWeapon.SetFileId("opt_weapon");
	OptionsGroup.PlaceLabledControl("Weapon", this, &OptionsWeapon);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl("Info", this, &OptionsInfo);

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("Off");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsGroup.PlaceLabledControl("Chams", this, &OptionsChams);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl("Skeleton", this, &OptionsSkeleton);

	OptionsAimSpot.SetFileId("opt_aimspot");
	OptionsGroup.PlaceLabledControl("Aim Spot", this, &OptionsAimSpot);
	
	OptionsCompRank.SetFileId("opt_comprank");
	OptionsGroup.PlaceLabledControl("Competitive Rank", this, &OptionsCompRank);

#pragma endregion Setting up the Options controls

#pragma region Filters
	FiltersGroup.SetText("Filters");
	FiltersGroup.SetPosition(225, 48);
	FiltersGroup.SetSize(193, 430);
	RegisterControl(&FiltersGroup);

	FiltersAll.SetFileId("ftr_all");
	FiltersGroup.PlaceLabledControl("All", this, &FiltersAll);

	FiltersPlayers.SetFileId("ftr_players");
	FiltersGroup.PlaceLabledControl("Players", this, &FiltersPlayers);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	FiltersGroup.PlaceLabledControl("Enemies Only", this, &FiltersEnemiesOnly);

	FiltersWeapons.SetFileId("ftr_weaps");
	FiltersGroup.PlaceLabledControl("Weapons", this, &FiltersWeapons);

	FiltersChickens.SetFileId("ftr_chickens");
	FiltersGroup.PlaceLabledControl("Chickens", this, &FiltersChickens);

	FiltersC4.SetFileId("ftr_c4");
	FiltersGroup.PlaceLabledControl("C4", this, &FiltersC4);
#pragma endregion Setting up the Filters controls

#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(434, 48);
	OtherGroup.SetSize(334, 430);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_xhair");
	OtherGroup.PlaceLabledControl("Crosshair", this, &OtherCrosshair);

	OtherRecoilCrosshair.SetFileId("otr_recoilhair");
	OtherRecoilCrosshair.AddItem("Off");
	OtherRecoilCrosshair.AddItem("Recoil Position");
	OtherRecoilCrosshair.AddItem("Radius");
	OtherGroup.PlaceLabledControl("Recoil Crosshair", this, &OtherRecoilCrosshair);

	OtherRadar.SetFileId("otr_radar");
	OtherGroup.PlaceLabledControl("Radar", this, &OtherRadar);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherNoSky.SetFileId("otr_nosky");
	OtherGroup.PlaceLabledControl("No Sky", this, &OtherNoSky);

	OtherNoFlash.SetFileId("otr_noflash");
	OtherGroup.PlaceLabledControl("No Flash", this, &OtherNoFlash);

	OtherNoSmoke.SetFileId("otr_nosmoke");
	OtherGroup.PlaceLabledControl("No Smoke", this, &OtherNoSmoke);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Transparent");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OtherGroup.PlaceLabledControl("Hands", this, &OtherNoHands);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceLabledControl("Auto Jump", this, &OtherAutoJump);

	OtherAutoStrafe.SetFileId("otr_autostrafe");
	OtherGroup.PlaceLabledControl("Auto Strafe", this, &OtherAutoStrafe);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl("Safe Mode", this, &OtherSafeMode);

	OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("Off");
	OtherChatSpam.AddItem("Round-Say");
	OtherChatSpam.AddItem("Regular");
	OtherChatSpam.AddItem("Report Text Spam");
	OtherGroup.PlaceLabledControl("Chat Spam", this, &OtherChatSpam);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	OtherSpectators.SetFileId("otr_speclist");
	OtherGroup.PlaceLabledControl("Spectators List", this, &OtherSpectators);
#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("Skins");

#pragma region Knife
	KnifeGroup.SetPosition(16, 16);
	KnifeGroup.SetSize(360, 400);
	KnifeGroup.SetText("Skin Changer");
	RegisterControl(&KnifeGroup);

	KnifeEnable.SetFileId("knife_enable");
	KnifeGroup.PlaceLabledControl("Enable", this, &KnifeEnable);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("Karambit");
	KnifeModel.AddItem("Bayonet");
	KnifeModel.AddItem("Huntsman Knife");
	KnifeModel.AddItem("Gut Knife");
	KnifeModel.AddItem("Walmart Knife");
	KnifeModel.AddItem("Shadow Daggers");
	KnifeModel.AddItem("Butterfly Knife");
	KnifeModel.AddItem("Flip Knife");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Damascus Steel");
	KnifeSkin.AddItem("Doppler Phase 1");
	KnifeSkin.AddItem("Doppler Phase 2");
	KnifeSkin.AddItem("Doppler Phase 3");
	KnifeSkin.AddItem("Doppler Phase 4");
	KnifeSkin.AddItem("Doppler (Ruby)");
	KnifeSkin.AddItem("Doppler (Sapphire)");
	KnifeSkin.AddItem("Doppler (Blackpearl)");
	KnifeSkin.AddItem("Marble Fade");
	KnifeSkin.AddItem("Tiger Tooth");
	KnifeSkin.AddItem("Ultraviolet");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Night");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);

	M41SSkin.SetFileId("m4a1s_skin");
	M41SSkin.AddItem("Masterpiece");
	M41SSkin.AddItem("Hyper Beast");
	M41SSkin.AddItem("Cyrex");
	M41SSkin.AddItem("Hot Rod");
	KnifeGroup.PlaceLabledControl("M4A1-S", this, &M41SSkin);

	M4A4Skin.SetFileId("m4a4_skin");
	M4A4Skin.AddItem("Royal Paladin");
	M4A4Skin.AddItem("Howl");
	M4A4Skin.AddItem("Bullet Rain");
	M4A4Skin.AddItem("Dragon King");
	M4A4Skin.AddItem("Asiimov");
	KnifeGroup.PlaceLabledControl("M4A4", this, &M4A4Skin);

	AK47Skin.SetFileId("ak47_skin");
	AK47Skin.AddItem("Point Disarray");
	AK47Skin.AddItem("Aquamarine Revenge");
	AK47Skin.AddItem("Vulcan");
	AK47Skin.AddItem("Fire Serpent");
	AK47Skin.AddItem("Jaguar");
	KnifeGroup.PlaceLabledControl("AK-47", this, &AK47Skin);

	AWPSkin.SetFileId("awp_skin");
	AWPSkin.AddItem("Hyper Beast");
	AWPSkin.AddItem("Asiimov");
	AWPSkin.AddItem("Dragon Lore");
	AWPSkin.AddItem("Lightning Strike");
	KnifeGroup.PlaceLabledControl("AWP", this, &AWPSkin);

	GLOCKSkin.SetFileId("glock_skin");
	GLOCKSkin.AddItem("Water Elemental");
	GLOCKSkin.AddItem("Fade");
	GLOCKSkin.AddItem("Twilight Galaxy");
	GLOCKSkin.AddItem("Bunsen Burner");
	KnifeGroup.PlaceLabledControl("Glock", this, &GLOCKSkin);

	USPSSkin.SetFileId("m4a1s_skin");
	USPSSkin.AddItem("Kill Confirmed");
	USPSSkin.AddItem("Caiman");
	USPSSkin.AddItem("Orion");
	USPSSkin.AddItem("Guardian");
	KnifeGroup.PlaceLabledControl("USP-S", this, &USPSSkin);

	MAGNUMSkin.SetFileId("357_skin");
	MAGNUMSkin.AddItem("Crimson Web");
	MAGNUMSkin.AddItem("Fade");
	MAGNUMSkin.AddItem("Amber Fade");
	KnifeGroup.PlaceLabledControl("R8 Revolver", this, &MAGNUMSkin);

	KnifeApply.SetText("Apply Changes");
	KnifeApply.SetCallback(KnifeApplyCallbk);
	KnifeGroup.PlaceLabledControl("", this, &KnifeApply);






#pragma endregion

#pragma endregion other random options
}

void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	GUI.Update();
	GUI.Draw();

	
}


