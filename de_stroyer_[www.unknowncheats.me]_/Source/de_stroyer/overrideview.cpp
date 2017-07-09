#include "sdk.h"

OverrideViewFn oOverrideView;
void __stdcall Hooks::OverrideView( CViewSetup* vsView )
{

	if(I::Engine->IsInGame( ))//Vars::Visuals::Enabled)
	{
		/*auto viewPunch = G::LocalPlayer->GetViewPunch();
		auto aimPunch = G::LocalPlayer->GetPunch();
		vsView->angles[0] += ((aimPunch[0] * .4499999f) + viewPunch[0]);// 0.4499999f));
		vsView->angles[1] += ((aimPunch[1] * .4499999f) + viewPunch[1]);
		vsView->angles[2] += ((aimPunch[2] * .4499999f) + viewPunch[2]);// 0.4499999f));
		/*vsView->angles[0] += (viewPunch[0] + (aimPunch[0] * .4499999f));// 0.4499999f));
		vsView->angles[1] += (viewPunch[1] + (aimPunch[1] * .4499999f));// 0.4499999f));
		vsView->angles[2] += (viewPunch[2] + (aimPunch[2] * .4499999f));// 0.4499999f));*/

		//if (Vars::Visuals::UseCustomFOV && Vars::Visuals::CustomFOV >= 1 && vsView->fov == 90)
		if(vsView->fov == 90)
			vsView->fov = Config->GetValue( "Visuals", "FOV Amount" ).m_flValue;
	}

	oOverrideView( vsView );
}