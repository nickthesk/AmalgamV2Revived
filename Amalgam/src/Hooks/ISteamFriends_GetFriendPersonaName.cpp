
#include "../SDK/SDK.h"

#include "../Features/Players/PlayerUtils.h"

MAKE_SIGNATURE(GetPlayerNameForSteamID_GetFriendPersonaName_Call, "client.dll", "41 B9 ? ? ? ? 44 8B C3 48 8B C8", 0x0);

MAKE_HOOK(ISteamFriends_GetFriendPersonaName, U::Memory.GetVirtual(I::SteamFriends, 7), const char*,
	void* rcx, CSteamID steamIDFriend)
{
	DEBUG_RETURN(ISteamFriends_GetFriendPersonaName, rcx, steamIDFriend);

	const auto dwRetAddr = uintptr_t(_ReturnAddress());
	const auto dwDesired = S::GetPlayerNameForSteamID_GetFriendPersonaName_Call();
//This trick theoretically allows you to use the name command (Only on community servers)
if (steamIDFriend == I::SteamUser->GetSteamID())
    {
        static ConVar* nameVar = I::CVar->FindVar("name");
        CTFPlayer* pLocal = H::Entities.GetLocal();
        bool bInGame = I::EngineClient->IsInGame() && pLocal; //to call the user's original name 
		//and prevent it from only using the last one used.
        // If we are not in a match, we return the real Steam name 
        if (!bInGame)
        {
            return CALL_ORIGINAL(rcx, steamIDFriend);
        }
        // If in-game, return the string from the "name" console variable
        if (nameVar)
        {
            return nameVar->GetString();
        }
    }

	if (dwRetAddr == dwDesired && Vars::Visuals::UI::StreamerMode.Value)
	{
		switch (F::PlayerUtils.GetNameType(steamIDFriend.GetAccountID()))
		{
		case NameTypeEnum::Local: return LOCAL;
		case NameTypeEnum::Friend: return FRIEND;
		case NameTypeEnum::Party: return PARTY;
		}
	}

	return CALL_ORIGINAL(rcx, steamIDFriend);
}
