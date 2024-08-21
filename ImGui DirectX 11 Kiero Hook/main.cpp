#include "includes.h"
#include "sdk.h"
#include "kiero/minhook/include/MinHook.h"
#include <vector>
#include <Windows.h>
#include <iostream>
#include <string>
#include "obfuscation.h"

typedef bool(__fastcall* CSessionPost)(void* pThis, CCommand* pCommand, bool ForceSend);
CSessionPost CSessionPostHook;
CSessionPost CSessionPostTramp;

typedef CAddPlayerCommand* (__fastcall* GetCAddPlayerCommand)(void* pThis, CString* User, CString* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7);
GetCAddPlayerCommand CAddPlayerCommandHook;
GetCAddPlayerCommand CAddPlayerCommandTramp;

typedef CStartGameCommand* (__fastcall* GetCStartGameCommand)(void* pThis);
GetCStartGameCommand CStartGameCommandFunc;
GetCStartGameCommand CStartGameCommandTramp;

typedef CRemovePlayerCommand* (__fastcall* GetCRemovePlayerCommand)(void* pThis, int nMachineId, int eReason, long long a4);
GetCRemovePlayerCommand CRemovePlayerCommandHook;
GetCRemovePlayerCommand CRemovePlayerCommandTramp;

typedef __int64(__fastcall* CGameStateSetPlayer)(void* pThis, int* Tag);
CGameStateSetPlayer CGameStateSetPlayerHook;
CGameStateSetPlayer CGameStateSetPlayerTramp;


typedef CNameChange* (__fastcall* GetCNameChangeCommand)(void* pThis, CString* name);
GetCNameChangeCommand CNameChangeFunc;
GetCNameChangeCommand CNameChangeTramp;

typedef CChatMessage* (__fastcall* GetCChatMessage)(void* pThis, __int64 a2, __int64 Message);
GetCChatMessage CChatMessageHook;
GetCChatMessage CChatMessageTramp;

typedef CCrash* (__fastcall* GetCCrash)(void* pThis, unsigned int a1);
GetCCrash CCrashFunc;
GetCCrash CCrashTramp;

typedef CCreateEquipmentVariant* (__fastcall* GetCCreateEquipmentVariant)(void* pThis, __int64 a2, __int64 a3, int a4, __int64* a5, char a6);
GetCCreateEquipmentVariant CCreateEquipmentFunc;
GetCCreateEquipmentVariant CCreateEquipmentTramp;

typedef CPauseGameCommand* (__fastcall* GetCPauseGameCommand)(void* pthis, __int64 a2, char a3);
GetCPauseGameCommand CPauseGameFunc;

typedef CAiEnableCommand* (__fastcall* GetEnableAI)(void* pThis, int* tag, int toggled);
GetEnableAI AIEnableFunc;

typedef CChatLeaveFake* (_fastcall* GetCChatLeaveFake)(void* pThis, int eReason);
GetCChatLeaveFake ChatLeaveFunc;

typedef LPVOID(__fastcall* GetCCommand)(__int64 a1);
GetCCommand GetCCommandFunc;

typedef CGameSpeed* (__fastcall* GetCGameSpeed)(void* pThis);
GetCGameSpeed IncreaseSpeedFunc;
GetCGameSpeed DecreaseSpeedFunc;

typedef CSetSpeed* (__fastcall* GetCGameSetSpeed)(void* pThis, int speed);
GetCGameSetSpeed SetSpeedFunc;

typedef EmptyTest* (__fastcall* GetCustomDiffM)(void* pThis, __int64 a2, int a3);
GetCustomDiffM GCDMF;
GetCustomDiffM GCDMH;

typedef EmptyTest* (__fastcall* GetCustomDiff)(void* pThis, int a2);
GetCustomDiff GetCustomF;
GetCustomDiff GetCustomH;

typedef EmptyTest* (__fastcall* GetAntiBan)(void* pThis, char a2);
GetAntiBan AntiBanFunc;
GetAntiBan AntiBanTramp;

int FakeM = 1;
int FakeM2 = 50;
bool FakeSpammer = false;

bool AllocatedConsole = false;
bool Debug = true;
bool bRefuseConnect = false;

//ChatMessage
//__int64 pCMessage = NULL;

//SessionPost
void* pCSession = nullptr;

//AddPlayerCommand
void* pCAddPlayer = nullptr;
DWORD* dUnknown;
int dMachine;
__int64 dPdx;
CString* dUser;
CString* dHname;
bool bSpoofSteam = false;
bool bJoinAsGhost = false;
int iMyMachineID;
int iMachineIDFake = 50;
__int64 iParadoxSocialID = 0;
CString* empty = new CString;

//RemovePlayerCommand
void* pCRemovePlayer = nullptr;
ERemovalReason dReason;
int RMID;
int dRUnknown;

//GameSetState
void* pCGameState = nullptr;

//Langauge
bool lEnglish = true;
bool lRussian = false;
bool HasNotChanged = false;


// We do l at the start to define it as languange (you dont have to do this but it makes it easier to read)
std::string lStartGame = "";



//ImGui
bool bMenuOpen = true;
bool bCE = false;
bool bCrasher = false;
bool bXP = false;
char TagBuffer[16];

//something something my favourite show is BFDI (Menus, obviously)
bool bLobbyMenu = false;
bool bGameMenu = false;
bool bSettingsMenu = false;

//Strengthen
int boost = 0;
__int64 CountryTag;

//Hook
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;
uintptr_t GameBase = (uintptr_t)GetModuleHandleA("hoi4.exe");


//Console
void printm(std::string str)
{
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);

	std::cout << "[SilverHook] " << str << std::endl;
}

//Cheats
void PatchMemory(uintptr_t address, unsigned char* patch, DWORD size)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((LPVOID)address, patch, size);
	VirtualProtect((LPVOID)address, size, oldProtect, &oldProtect);
}


void MultiplayerLobbyHack()
{

	uintptr_t address = GameBase + 0x185CD2F;

	unsigned char patch[] = { 0x74, 0x0F, 0x83, 0xFA, 0x01, 0x00 }; 

	void* MLHmem = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (MLHmem != nullptr)
	{
		DWORD relativeOffset = (GameBase + 0x185CD40) - (address + sizeof(patch));
		memcpy(MLHmem, patch, sizeof(patch));
		*(BYTE*)((uintptr_t)MLHmem) = 0x0F;
		*(BYTE*)((uintptr_t)MLHmem + 1) = 0x83;
		*(DWORD*)((uintptr_t)MLHmem + 2) = relativeOffset;

		PatchMemory(address, (unsigned char*)MLHmem, sizeof(patch));
		VirtualFree(MLHmem, 0, MEM_RELEASE);
	}

}

//void cmp() {
	//uintptr_t address = GameBase + 0x185CD35;

	//unsigned char patch[] = { 0x74, 0x0F, 0x83, 0xFA, 0x01, 0x00 };

	//void* MLHmem = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//}

void UpgradeLevelLimiter() {

	uintptr_t addr = GameBase + 0x1B1C1E0;

	unsigned char newValue = 0x99;

	unsigned char newCode[] = {
		0x41, 0xC7, 0x85, 0x0C, 0x01, 0x00, 0x00, newValue, 0x00,0x00, 0x00,  
		  
		0x90, 0x90, 0x90, 0x90   
	};

	
	PatchMemory(addr, newCode, sizeof(newCode));
	

}


/*void* __cdecl mBase(size_t Size) {
	size_t i;
	void* result;
	i = Size;
	if (Size > 0xFFFFFFFFFFFFFFE0ui64)
	{
		printm("Malloc Error");
		return 0i64;
	}
	else
	{
		if (!Size)
			i = 1i64;
		while (1)
		{
			result = HeapAlloc(0, 0, i);
			if (result)
				break;
			if (!_callnewh(i)) {
				printm("Malloc Error");
				return 0i64;
			}
		}
	}
	return result;
}*/


void* __fastcall SizeF(size_t Size) {
	size_t i;
	void* result;
	

	for (i = Size; ; Size = i) {
		result = _malloc_base(Size);
		if (result) {
			break;
		}
		if (!_callnewh(i)) {
			if (i != -1i64) {
				printm("Bad Allocation");
			}
			printm("Bad Array Length");
		}
	}
	return result;
}



template <typename T>
T ReadMemory(uintptr_t address)
{
	return *reinterpret_cast<T*>(address);
}

uintptr_t OffsetCalculator(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets)
{
	uintptr_t address = baseAddress;
	for (uintptr_t offset : offsets)
	{
		address = ReadMemory<uintptr_t>(address);
		address += offset;
	}
	return address;
}



void ChangeIntAddressValue(uintptr_t bAddr, uintptr_t bOff, int Tag)
{

	uintptr_t baseAddress = bAddr; 
	
	std::vector<uintptr_t> offsets = { bOff };
	
	uintptr_t finalAddress = OffsetCalculator(GameBase + baseAddress, offsets);

	DWORD* pValue = reinterpret_cast<DWORD*>(finalAddress);
	DWORD oldProtect;

	VirtualProtect(pValue, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect);

	*pValue = Tag;

	VirtualProtect(pValue, sizeof(DWORD), oldProtect, &oldProtect);

}

void ChangeByteAddressValue(uintptr_t addr)
{
	uintptr_t address = GameBase + addr; 
	BYTE* pValue = (BYTE*)address;
	DWORD oldProtect;

	VirtualProtect(pValue, sizeof(BYTE), PAGE_EXECUTE_READWRITE, &oldProtect);
	
	if (*pValue == 1)
	{
		*pValue = 0;
	}
	else
	{
		*pValue = 1;
	}
	
	VirtualProtect(pValue, sizeof(BYTE), oldProtect, &oldProtect);
}


//IngameFunctions
void Crasher(int a1) {
	CCrash* x = (CCrash*)GetCCommandFunc(48);
	x = CCrashFunc(x, a1);
	CSessionPostTramp(pCSession, x, true);
}


void StartGameFunc() {

	//Using the manually made function instead lol!
	CStartGameCommand* StartGame = (CStartGameCommand*)GetCCommandFunc(41);

	StartGame = CStartGameCommandFunc((StartGame));
	CSessionPostTramp(pCSession, StartGame, true);
}

void RemovalReason(ERemovalReason e) {
	//Only host can remove themselves? (possibly machine ID)
	CRemovePlayerCommand* RemovePlayer = (CRemovePlayerCommand*)GetCCommandFunc(41);
	RemovePlayer = CRemovePlayerCommandTramp(RemovePlayer, RMID, e, dRUnknown);
	CSessionPostTramp(pCSession, RemovePlayer, true);
}



void DefaultImGui() {
	ImVec4 NormalColor = ImVec4(0.047f, 0.047f, 0.047f, 1.0f);
	ImVec4 HoverColor = ImVec4(0.066f, 0.066f, 0.066f, 1.0f);
	ImVec4 ActiveColor = ImVec4(0.076f, 0.076f, 0.076f, 1.0f);
	ImVec4 BackgroundColor = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
	ImVec4 PureBlackColor = ImVec4(0.01f, 0.01f, 0.01f, 1.0f);
	ImVec4 WhiteColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = NormalColor;  // Normal Colors
	style.Colors[ImGuiCol_FrameBg] = NormalColor;
	style.Colors[ImGuiCol_ResizeGrip] = NormalColor;
	style.Colors[ImGuiCol_Separator] = NormalColor;

	style.Colors[ImGuiCol_ButtonHovered] = HoverColor; //Hovered Colors
	style.Colors[ImGuiCol_ResizeGripHovered] = HoverColor;
	style.Colors[ImGuiCol_FrameBgHovered] = HoverColor;
	style.Colors[ImGuiCol_SeparatorHovered] = HoverColor;

	style.Colors[ImGuiCol_ButtonActive] = ActiveColor; // Active Colors
	style.Colors[ImGuiCol_ResizeGripActive] = ActiveColor;
	style.Colors[ImGuiCol_SeparatorActive] = ActiveColor;
	style.Colors[ImGuiCol_FrameBgActive] = ActiveColor;

	style.Colors[ImGuiCol_WindowBg] = BackgroundColor; // Background

	style.Colors[ImGuiCol_TitleBg] = PureBlackColor; // Title
	style.Colors[ImGuiCol_TitleBgCollapsed] = PureBlackColor;
	style.Colors[ImGuiCol_TitleBgActive] = PureBlackColor;

	style.Colors[ImGuiCol_CheckMark] = WhiteColor; // Checkmark
}

__int64* TagFixFunc(__int64* a1, __int64 a2) {

	__int64* v2;
	__int64* v3;
	unsigned __int64 v5;
	__int64 v6;
	void* v7;
	void* v8;
	__int64* result;

	v2 = 0i64;
	v3 = (__int64*)a2;
	*a1 = 0i64;
	a1[2] = 0i64;
	a1[3] = 0i64;
	v5 = *(__int64*)(a2 + 16);
	if (*(__int64*)(a2 + 24) >= 16ui64)
		v3 = *(__int64**)a2;
	if (v5 >= 0x10)
	{
		v6 = v5 | 0xF;
		if ((v5 | 0xF) > 0x7FFFFFFFFFFFFFFFi64)
			v6 = 0x7FFFFFFFFFFFFFFFi64;
		if ((unsigned __int64)(v6 + 1) < 0x1000)
		{
			if (v6 != -1)
				v2 = (__int64*)SizeF(v6 + 1);
		}
		else
		{
			if (v6 + 40 <= (unsigned __int64)(v6 + 1))
				printm("Bad Array Type");
			v7 = SizeF(v6 + 40);
			v8 = v7;
			v2 = (__int64*)(((unsigned __int64)v7 + 39) & 0xFFFFFFFFFFFFFFE0ui64);
			*(v2 - 1) = (__int64)v8;
		}
		*a1 = *v2;
		typedef EmptyTest* (__fastcall* Func)(__int64* a1, __int64* a2, unsigned __int64 a3);
		Func RandFunc;

		RandFunc = Func(GameBase + 0x2136D40);
		RandFunc(v2, v3, v5 + 1);
	}
	else
	{
		v6 = 15i64;
		*(__int64*)a1 = *v3;
	}
	a1[2] = v5;
	result = a1;
	a1[3] = v6;
	return result;

	//return *(__int64*)(a1 + 16) + 56i64;
}




void AnnexCountry(int tag) {


	//Disgusting HackCode


	int D2H4DJESOA7CJ = 251367161;
	if (D2H4DJESOA7CJ > 251367154)
		D2H4DJESOA7CJ = 251367128;
	else if (D2H4DJESOA7CJ <= 251367168)
		D2H4DJESOA7CJ++;
	else
		D2H4DJESOA7CJ = (251367179 / 251367166);
	bool DG6SK98740CKP = true;
	if (!DG6SK98740CKP)
		DG6SK98740CKP = true;
	else if (DG6SK98740CKP = true)
		DG6SK98740CKP = true;
	else
		DG6SK98740CKP = true;
	bool D16R2A4ZA5WKR = false;
	if (!D16R2A4ZA5WKR)
		D16R2A4ZA5WKR = true;
	else if (D16R2A4ZA5WKR = true)
		D16R2A4ZA5WKR = true;
	else
		D16R2A4ZA5WKR = true;
	int DDSES5C3IXB0B = 251367187;
	if (DDSES5C3IXB0B > 251367118)
		DDSES5C3IXB0B = 251367108;
	else if (DDSES5C3IXB0B <= 251367123)
		DDSES5C3IXB0B++;
	else
		DDSES5C3IXB0B = (251367110 / 251367107);
	int DJD55RON4Y04R = 251367109;
	if (DJD55RON4Y04R > 251367194)
		DJD55RON4Y04R = 251367155;
	else if (DJD55RON4Y04R <= 251367133)
		DJD55RON4Y04R++;
	else
		DJD55RON4Y04R = (251367117 / 251367112);
	bool D9L8D16I270S3 = true;
	if (!D9L8D16I270S3)
		D9L8D16I270S3 = false;
	else if (D9L8D16I270S3 = true)
		D9L8D16I270S3 = true;
	else
		D9L8D16I270S3 = true;
	bool DBA11L04RS4HA = false;
	if (!DBA11L04RS4HA)
		DBA11L04RS4HA = false;
	else if (DBA11L04RS4HA = true)
		DBA11L04RS4HA = false;
	else
		DBA11L04RS4HA = false;
	bool D8YI6PBH70M7H = true;
	if (!D8YI6PBH70M7H)
		D8YI6PBH70M7H = false;
	else if (D8YI6PBH70M7H = true)
		D8YI6PBH70M7H = true;
	else
		D8YI6PBH70M7H = true;
	int D93IEQGCBJP4L = 251367116;
	if (D93IEQGCBJP4L > 251367193)
		D93IEQGCBJP4L = 251367102;
	else if (D93IEQGCBJP4L <= 251367125)
		D93IEQGCBJP4L++;
	else
		D93IEQGCBJP4L = (251367157 / 251367177);
	bool DF5BGNPC2HYZY = true;
	if (!DF5BGNPC2HYZY)
		DF5BGNPC2HYZY = true;
	else if (DF5BGNPC2HYZY = false)
		DF5BGNPC2HYZY = true;
	else
		DF5BGNPC2HYZY = true;
	bool D2WM5I40F5N7G = true;
	if (!D2WM5I40F5N7G)
		D2WM5I40F5N7G = true;
	else if (D2WM5I40F5N7G = true)
		D2WM5I40F5N7G = false;
	else
		D2WM5I40F5N7G = true;
	int DHHY670IZGMSZ = 251367112;
	if (DHHY670IZGMSZ > 251367160)
		DHHY670IZGMSZ = 251367166;
	else if (DHHY670IZGMSZ <= 251367162)
		DHHY670IZGMSZ++;
	else
		DHHY670IZGMSZ = (251367195 / 251367179);
	int DOSOO6KDSJ525 = 251367155;
	if (DOSOO6KDSJ525 > 251367143)
		DOSOO6KDSJ525 = 251367140;
	else if (DOSOO6KDSJ525 <= 251367191)
		DOSOO6KDSJ525++;
	else
		DOSOO6KDSJ525 = (251367160 / 251367183);
	bool D3GPX093G35EQ = true;
	if (!D3GPX093G35EQ)
		D3GPX093G35EQ = true;
	else if (D3GPX093G35EQ = true)
		D3GPX093G35EQ = false;
	else
		D3GPX093G35EQ = false;
	bool DGELE95I8DP95 = false;
	if (!DGELE95I8DP95)
		DGELE95I8DP95 = false;
	else if (DGELE95I8DP95 = true)
		DGELE95I8DP95 = false;
	else
		DGELE95I8DP95 = true;
	int DGCEZXPN9FKIS = 251367129;
	if (DGCEZXPN9FKIS > 251367167)
		DGCEZXPN9FKIS = 251367193;
	else if (DGCEZXPN9FKIS <= 251367155)
		DGCEZXPN9FKIS++;
	else
		DGCEZXPN9FKIS = (251367119 / 251367167);
	int D09KC604Z9GOC = 251367136;
	if (D09KC604Z9GOC > 251367112)
		D09KC604Z9GOC = 251367143;
	else if (D09KC604Z9GOC <= 251367131)
		D09KC604Z9GOC++;
	else
		D09KC604Z9GOC = (251367177 / 251367135);
	bool D7MKKCJ7OE5CW = true;
	if (!D7MKKCJ7OE5CW)
		D7MKKCJ7OE5CW = false;
	else if (D7MKKCJ7OE5CW = true)
		D7MKKCJ7OE5CW = false;
	else
		D7MKKCJ7OE5CW = true;
	bool DE78OSX4PF3X6 = false;
	if (!DE78OSX4PF3X6)
		DE78OSX4PF3X6 = true;
	else if (DE78OSX4PF3X6 = false)
		DE78OSX4PF3X6 = true;
	else
		DE78OSX4PF3X6 = false;
	int DG2H1W3M9CMGE = 251367122;
	if (DG2H1W3M9CMGE > 251367129)
		DG2H1W3M9CMGE = 251367171;
	else if (DG2H1W3M9CMGE <= 251367176)
		DG2H1W3M9CMGE++;
	else
		DG2H1W3M9CMGE = (251367125 / 251367192);
	bool DALD5LWQZ09AE = false;
	if (!DALD5LWQZ09AE)
		DALD5LWQZ09AE = true;
	else if (DALD5LWQZ09AE = false)
		DALD5LWQZ09AE = true;
	else
		DALD5LWQZ09AE = false;
	bool DC2JQWRCERB0P = true;
	if (!DC2JQWRCERB0P)
		DC2JQWRCERB0P = true;
	else if (DC2JQWRCERB0P = false)
		DC2JQWRCERB0P = true;
	else
		DC2JQWRCERB0P = false;
	int DRXHCRLWNISB8 = 251367190;
	if (DRXHCRLWNISB8 > 251367107)
		DRXHCRLWNISB8 = 251367188;
	else if (DRXHCRLWNISB8 <= 251367160)
		DRXHCRLWNISB8++;
	else
		DRXHCRLWNISB8 = (tag / 251367163);
	int DNQ0H7PF1G1FA = 251367170;
	if (DNQ0H7PF1G1FA > 251367126)
		DNQ0H7PF1G1FA = 251367154;
	else if (DNQ0H7PF1G1FA <= 251367146)
		DNQ0H7PF1G1FA++;
	else
		DNQ0H7PF1G1FA = (251367126 / 251367101);
	bool D55H41HJE700J = false;
	if (!D55H41HJE700J)
		D55H41HJE700J = true;
	else if (D55H41HJE700J = true)
		D55H41HJE700J = true;
	else
		D55H41HJE700J = true;
	int D4ZKM96EG7EN0 = 251367178;
	if (D4ZKM96EG7EN0 > 251367160)
		D4ZKM96EG7EN0 = 251367129;
	else if (D4ZKM96EG7EN0 <= 251367175)
		D4ZKM96EG7EN0++;
	else
		D4ZKM96EG7EN0 = (251367195 / 251367186);
	bool D6CZ693M604QP = true;
	if (!D6CZ693M604QP)
		D6CZ693M604QP = true;
	else if (D6CZ693M604QP = false)
		D6CZ693M604QP = true;
	else
		D6CZ693M604QP = true;
	bool DIG4SR0I5E3FN = true;
	if (!DIG4SR0I5E3FN)
		DIG4SR0I5E3FN = false;
	else if (DIG4SR0I5E3FN = true)
		DIG4SR0I5E3FN = false;
	else
		DIG4SR0I5E3FN = true;
	int DAR13LN9XH2CO = 251367111;
	if (DAR13LN9XH2CO > 251367194)
		DAR13LN9XH2CO = 251367131;
	else if (DAR13LN9XH2CO <= 251367176)
		DAR13LN9XH2CO++;
	else
		DAR13LN9XH2CO = (251367131 / 251367178);
	int DFHM2833G8RWY = 251367111;
	if (DFHM2833G8RWY > 251367147)
		DFHM2833G8RWY = 251367125;
	else if (DFHM2833G8RWY <= 251367124)
		DFHM2833G8RWY++;
	else
		DFHM2833G8RWY = (251367104 / 251367197);
	int DJ5M0EMKW0GXP = 251367171;
	if (DJ5M0EMKW0GXP > 251367115)
		DJ5M0EMKW0GXP = 251367153;
	else if (DJ5M0EMKW0GXP <= 251367193)
		DJ5M0EMKW0GXP++;
	else
		DJ5M0EMKW0GXP = (251367157 / 251367153);
	bool DJMPPX18D4KX6 = true;
	if (!DJMPPX18D4KX6)
		DJMPPX18D4KX6 = true;
	else if (DJMPPX18D4KX6 = true)
		DJMPPX18D4KX6 = true;
	else
		DJMPPX18D4KX6 = false;
}

	


void InitImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic()); // For Russian
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
	
	DefaultImGui();

}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			InitImGui();
			ImGui::SetNextWindowSize(ImVec2(350, 1050));
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	if (GetAsyncKeyState(VK_INSERT) & 1)
		bMenuOpen = !bMenuOpen;

	if (GetAsyncKeyState(VK_HOME) & 1)
		StartGameFunc();

	if (bMenuOpen)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureMouse = true;
		io.WantCaptureKeyboard = true;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
		}
		else
		{
			io.MouseReleased[0] = true;
			io.MouseDown[0] = false;
			io.MouseClicked[0] = false;
		}





		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Dankus Admin Menu", &bMenuOpen);
		
		ImGui::Text("Language");
		if (ImGui::Checkbox("English", &lEnglish))
		{
			if (lEnglish)
				lRussian = false;
			HasNotChanged = false;

			
		
		};
		ImGui::SameLine();

		// NOT THIS
		if (ImGui::Checkbox(u8"Russian (Unfinished)", &lRussian))
		{
			if (lRussian)
				lEnglish = false;
			HasNotChanged = false;
			
		};

		ImGui::Text("Menus");
		if (ImGui::Checkbox(u8"Lobby Menu", &bLobbyMenu))
		{
			bLobbyMenu = true;
			bGameMenu = false;
			bSettingsMenu = false;
		}; 
		
		ImGui::SameLine();
		if (ImGui::Checkbox(u8"Ingame Menu", &bGameMenu))
		{
			bLobbyMenu = false;
			bGameMenu = true;
			bSettingsMenu = false;
		}; 
		if (ImGui::Checkbox(u8"Debug Menu", &bSettingsMenu))
		{
			bLobbyMenu = false;
			bGameMenu = false;
			bSettingsMenu = true;
		}; 



		//English
		if (lEnglish && !HasNotChanged) {
			HasNotChanged = true;

			//doing u8 first means it can read russian text
			lStartGame = u8"Start Game";
		}

		//Russian
		if (lRussian && !HasNotChanged) {
			HasNotChanged = true;
			lStartGame = u8"Начать игру";
		}
		if (bLobbyMenu)
		{
			ImGui::Text("Cheats");
			ImGui::Checkbox("Spoof Steam Name", &bSpoofSteam);
			ImGui::Checkbox("Join as Ghost", &bJoinAsGhost);
			ImGui::Checkbox("Multiplayer Lobby Hack", &bCE);	
			ImGui::Checkbox("FakeSpammer", &FakeSpammer);
			ImGui::Text("Function Calls");
			ImGui::Text("");
			ImGui::Columns(2);
			//Must do c_str() to turn string into char array
			if (ImGui::Button(lStartGame.c_str(), ImVec2(140, 28)) && pCSession != nullptr)
			{

				StartGameFunc();

			}
			ImGui::NextColumn();
			if (ImGui::Button("Reset Fake MachineID", ImVec2(140, 28)) && pCSession != nullptr)
			{

				FakeM = 1;

			}
			ImGui::NextColumn();
			if (ImGui::Button("Add Fake Player", ImVec2(140, 28)) && pCSession != nullptr)
			{
				DWORD* tt = dUnknown;
				__int64 tP = dPdx;
				CString* tN = dUser;
				CString* tNN = dHname;
				CString* empty = new CString;

				CAddPlayerCommand* AddFake = (CAddPlayerCommand*)GetCCommandFunc(200);

				FakeM2++;

				tN = (CString*)memcpy(tN, "SilverHook", 10);
				tNN = (CString*)memcpy(tNN, "RSilverXK 4on YT" ,18);
				AddFake = CAddPlayerCommandTramp(AddFake, tN, tNN, tt, FakeM2, false, tP);


				CSessionPostTramp(pCSession, AddFake, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Become Ghost", ImVec2(140, 28)) && pCSession != nullptr) {
				/*DWORD* tt = dUnknown;
				__int64 tP = dPdx;
				CString* tN = dUser;
				CString* tNN = dHname;

				CAddPlayerCommand* FakeKick = (CAddPlayerCommand*)GetCCommandFunc(200);
				FakeKick = CAddPlayerCommandTramp(FakeKick, tN, tNN, tt, tM, false, tP);
				CSessionPostTramp(pCSession, FakeKick, true);*/

				CRemovePlayerCommand* RemovePlayer = (CRemovePlayerCommand*)GetCCommandFunc(200);
				RemovePlayer = CRemovePlayerCommandTramp(RemovePlayer, dMachine, 0, dRUnknown);
				CSessionPostTramp(pCSession, RemovePlayer, true);

				//THIS ONE IS ANNOYING, PRETTY SURE THE COMMENTED OUT FUNCTION WORKS BETTER!
				
			}
			ImGui::Columns(1);
			ImGui::Text("");
			ImGui::Text("Credits: John Paradox");
		}
		if (bGameMenu) {
			ImGui::Text("Cheats");
			ImGui::Checkbox("NoXPCost + 99 Max Level (Testing)", &bXP);
			ImGui::Checkbox("Crasher", &bCrasher);
			ImGui::Checkbox("Antiban", &bRefuseConnect);
			ImGui::Text("Only use crasher when\ngame has finished loading.");
			ImGui::Text("Function Calls");
			ImGui::Text("");
			ImGui::Columns(2);
			if (ImGui::Button("Enable AI on all", ImVec2(140, 28)) && pCSession != nullptr)
			{
				int i = 0;
				do {
					int* TagPtr = &i;
					CAiEnableCommand* EnableAI = (CAiEnableCommand*)GetCCommandFunc(56);
					EnableAI = AIEnableFunc(EnableAI, TagPtr, 2);
					CSessionPostTramp(pCSession, EnableAI, 1);
					i++;
				} while (i <= 100);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Disable AI on all", ImVec2(140, 28)) && pCSession != nullptr)
			{
				int i = 0;
				do {
					int* TagPtr = &i;
					CAiEnableCommand* EnableAI = (CAiEnableCommand*)GetCCommandFunc(56);
					EnableAI = AIEnableFunc(EnableAI, TagPtr, 0);
					CSessionPostTramp(pCSession, EnableAI, 1);
					i++;
				} while (i <= 100);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Infinite Pause", ImVec2(140, 28)) && pCSession != nullptr) {
				CString* empty = new CString;
				CPauseGameCommand* UnpauseGame = (CPauseGameCommand*)GetCCommandFunc(88);
				UnpauseGame = CPauseGameFunc(UnpauseGame, (__int64)empty, 1);
				CSessionPostTramp(pCSession, UnpauseGame, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Ghost Pause", ImVec2(140, 28)) && pCSession != nullptr) {
				CString* empty = new CString;
				CPauseGameCommand* UnpauseGame = (CPauseGameCommand*)GetCCommandFunc(88);
				UnpauseGame = CPauseGameFunc(UnpauseGame, (__int64)empty, 0);
				CSessionPostTramp(pCSession, UnpauseGame, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Decrease Speed", ImVec2(140, 28)) && pCSession != nullptr) {
				CGameSpeed* DecreaseSpeed = (CGameSpeed*)GetCCommandFunc(80);
				DecreaseSpeed = DecreaseSpeedFunc(DecreaseSpeed);
				CSessionPostTramp(pCSession, DecreaseSpeed, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Increase Speed", ImVec2(140, 28)) && pCSession != nullptr) {
				CGameSpeed* IncreaseSpeed = (CGameSpeed*)GetCCommandFunc(48);
				IncreaseSpeed = IncreaseSpeedFunc(IncreaseSpeed);
				CSessionPostTramp(pCSession, IncreaseSpeed, true);
			}
			ImGui::Columns(1);
			ImGui::Text("");
			ImGui::Text("Mem Addresses");
			ImGui::Columns(2);

			if (ImGui::Button("FOW", ImVec2(140, 28)) && pCSession != nullptr)
			{
				ChangeByteAddressValue(0x2AB9DDA);
			}
			ImGui::NextColumn();
			if (ImGui::Button("AllowTraits", ImVec2(140, 28)) && pCSession != nullptr)
			{
				ChangeByteAddressValue(0x2AB9DB8);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Debug", ImVec2(140, 28)) && pCSession != nullptr)
			{
				ChangeByteAddressValue(0x2C96BEC);
			}

			ImGui::Columns(1);
			ImGui::Text("");
			ImGui::Text("Country Tag:");
			ImGui::SetNextItemWidth(70.f);
			ImGui::InputText("", TagBuffer, IM_ARRAYSIZE(TagBuffer));
			ImGui::SameLine();

			if (ImGui::Button("Enable AI") && pCSession != nullptr)
			{
				std::string sTagBuffer = TagBuffer;
				char* endptr;
				int a1 = std::strtol(TagBuffer, &endptr, 10);

				if (sTagBuffer.length() > 0)
				{
					int* TagPtr = &a1;
					CAiEnableCommand* EnableAI = (CAiEnableCommand*)GetCCommandFunc(56);
					EnableAI = AIEnableFunc(EnableAI, TagPtr, 2);
					CSessionPostTramp(pCSession, EnableAI, 1);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Disable AI") && pCSession != nullptr)
			{
				std::string sTagBuffer = TagBuffer;
				char* endptr;
				int a1 = std::strtol(TagBuffer, &endptr, 10);
				if (sTagBuffer.length() > 0)
				{

					int* TagPtr = &a1;
					CAiEnableCommand* DisableAI = (CAiEnableCommand*)GetCCommandFunc(56);
					DisableAI = AIEnableFunc(DisableAI, TagPtr, 0);
					CSessionPostTramp(pCSession, DisableAI, 1);
				}
			}
			if (ImGui::Button("Tagswitch") && pCSession != nullptr)
			{
				std::string sTagBuffer = TagBuffer;

				if (sTagBuffer.length() > 0)
				{
					int Tag = std::stoi(sTagBuffer);
					ChangeIntAddressValue(0x2C97110, 0x4B0, Tag);
				}
			}
			if (ImGui::Button("Annex") && pCSession != nullptr)
			{
				std::string sTagBuffer = TagBuffer;

				if (sTagBuffer.length() > 0)
				{
					int Tag = std::stoi(sTagBuffer);
					AnnexCountry(Tag);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				memset(TagBuffer, 0, sizeof(TagBuffer));
			}
			ImGui::Text("");
			ImGui::Text("Credits: John Paradox");
		}
		if (bSettingsMenu) {
			ImGui::Text("");
			ImGui::Checkbox("Debug", &Debug);
			ImGui::Text("Enabling debug WILL bring up a console\n as a seperate window. \nDO NOT CLOSE IT!");
			ImGui::Text("Boost Amount:");
			ImGui::SetNextItemWidth(70.f);
			ImGui::InputText("", TagBuffer, IM_ARRAYSIZE(TagBuffer));
			ImGui::SameLine();
			if (ImGui::Button("Boost") && pCSession != nullptr)
			{
				std::string sTagBuffer = TagBuffer;
				boost = std::stoi(TagBuffer);

				if (sTagBuffer.length() > 0)
				{
					EmptyTest* Strength = (EmptyTest*)GetCCommandFunc(88);
					Strength = GCDMF(Strength, CountryTag, boost);
					CSessionPostTramp(pCSession, Strength, true);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("200k"))
			{
				boost = 200000;
				
				EmptyTest* Strength = (EmptyTest*)GetCCommandFunc(88);
				
				Strength = GCDMF(Strength, CountryTag, boost);
				
				CSessionPostTramp(pCSession, Strength, true);

				memset(TagBuffer, 0, sizeof(TagBuffer));
			}
			if (ImGui::Button("speed"))
			{
				std::string sTagBuffer = TagBuffer;
				boost = std::stoi(TagBuffer);

				if (sTagBuffer.length() > 0)
				{
					CSetSpeed* Speed = (CSetSpeed*)GetCCommandFunc(56);
					Speed = SetSpeedFunc(Speed, boost);
					CSessionPostTramp(pCSession, Speed, true);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				boost = 0;
				EmptyTest* Strength = (EmptyTest*)GetCCommandFunc(88);
				Strength = GCDMF(Strength, CountryTag, boost);
				CSessionPostTramp(pCSession, Strength, true);
				memset(TagBuffer, 0, sizeof(TagBuffer));
			}
			ImGui::Text("CountryTag: ");
			ImGui::SameLine();
			std::string tag = std::to_string(CountryTag);
			ImGui::Text(tag.c_str());
			ImGui::SameLine();
			ImGui::Text("");
			ImGui::Text("Credits: John Paradox");
		}
		
		



		if (Debug) {
			if (!AllocatedConsole) {
				AllocConsole();
				AllocatedConsole = true;
			}
		}
		
		
		if (bCrasher) {
			Crasher(0);
		}
		if (bCE) {
			MultiplayerLobbyHack();
		}
		if (bXP) {
			UpgradeLevelLimiter();
		}
		if (FakeSpammer) {
			DWORD* a3 = dUnknown;
			__int64 a4 = dPdx;
			CString* a1 = dUser;
			CString* a2 = dHname;
			CString* empty = new CString;
			CAddPlayerCommand* AddFake = (CAddPlayerCommand*)GetCCommandFunc(112);
			FakeM++;
			(CString*)memcpy(a1, "SilverXK", 8);
			(CString*)memcpy(a2, "RFUWG", 8);
			AddFake = CAddPlayerCommandTramp(AddFake, a1, a2, a3, FakeM, false, a4);
			CSessionPostTramp(pCSession, AddFake, true);
		}


		ImGui::End();
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}


EmptyTest* __fastcall hkAntiBan(void* pThis, char a2) {

	printm("Ban Called");

	if (!bRefuseConnect)
		return AntiBanTramp(pThis, a2);
}


bool __fastcall hkCSessionPost(void* pThis, CCommand* pCommand, bool ForceSend)
{
	
	pCSession = pThis;
	if (Debug) {
		printm("CSP Called");
	}
	return CSessionPostTramp(pThis, pCommand, ForceSend);
}

CAddPlayerCommand* __fastcall hkCAddPlayerCommand(void* pThis, CString* User, CString* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7)
{
	if (iParadoxSocialID == 0)
		iParadoxSocialID = a7;

	iMachineIDFake = 50;
	iMyMachineID = nMachineId;

	if (bSpoofSteam)
	{
		User = empty;
	}

	if (bJoinAsGhost)
	{
		User = empty;
		Name = empty;
	}
	
	pCAddPlayer = pThis;
	dUser = User;
	dHname = Name;
	dUnknown = unknown;
	dMachine = nMachineId;
	dPdx = a7;

	return CAddPlayerCommandTramp(pThis, User, Name, unknown, nMachineId, bHotjoin, a7);
}

CRemovePlayerCommand* __fastcall hkCRemovePlayerCommand(void* pThis, int _machineID, ERemovalReason eReason, long long a4)
{

	pCRemovePlayer = pThis; RMID = _machineID; dReason = eReason; dRUnknown = a4;
	
	if (Debug) {
		printm("RPC Called");
	}
	return CRemovePlayerCommandTramp(pThis, _machineID, eReason, a4);
}

__int64 __fastcall hkCGameStateSetPlayer(void* pThis, int* Tag)
{

	pCGameState = pThis;
	if (Debug) {
		printm("CGSSP Called");
	}
	return CGameStateSetPlayerTramp(pThis, Tag);
}

/*CChatMessage* __fastcall hkChatMessage(void* pThis, __int64 a2, __int64 Message) {

	pCMessage = a2;
	if (Debug) {
		printm("CM Called");
	}
	return CChatMessageTramp(pThis, a2, Message);
}*/


/*CNameChange* __fastcall hkNameChange(void* pThis, CString* name) {


	if (Debug) {
		printm("CNC Called");
	}
	return CNameChangeTramp(pThis, name);
}*/

CCreateEquipmentVariant* __fastcall hkCCreateEquipment(void* pThis, __int64 a2, __int64 a3, int a4, __int64* a5, char a6) {

	if (bXP) {
		a4 = 0;
	}
	if (Debug) {
		printm("CCE Called");
	}
	return CCreateEquipmentTramp(pThis, a2, a3, a4, a5, a6);
}


EmptyTest* __fastcall hkCustomDiffM(void* pThis, __int64 Tag, int Boost) {
	if (Debug) {
		printm("DiffMultipley");
		std::string x;
		//x = std::to_string(a2);
		printm((char*)Tag);
		//a3 = boost;
		x = std::to_string(Boost);
		printm(x);
	}
	CountryTag = Tag;

	return GCDMH(pThis, Tag, Boost);
}
EmptyTest* __fastcall hkCustomDiff(void* pThis, int a2) {

	if (Debug) {
		printm("DiffChange");
	}
	return GetCustomH(pThis, a2);
}

CStartGameCommand* __fastcall hkStartGame(void* pThis) {

	if (Debug) {
		printm("StartGame Called");

	}

	return CStartGameCommandTramp(pThis);
}

CCrash* __fastcall hkCrash(void* pThis, unsigned int a1) {

	if (Debug) {
		printm("Crash Called");
		std::string x = std::to_string(a1);
		printm(x);
	}


	return CCrashTramp(pThis, a1);
}



void HookFunctions() {

	CSessionPostHook = CSessionPost(GameBase + 0x1DE2AF0);
	MH_CreateHook(CSessionPostHook, &hkCSessionPost, (LPVOID*)&CSessionPostTramp);
	MH_EnableHook(CSessionPostHook);

	CAddPlayerCommandHook = GetCAddPlayerCommand(GameBase + 0x1650E80);
	MH_CreateHook(CAddPlayerCommandHook, &hkCAddPlayerCommand, (LPVOID*)&CAddPlayerCommandTramp);
	MH_EnableHook(CAddPlayerCommandHook);
	


	CGameStateSetPlayerHook = CGameStateSetPlayer(GameBase + 0x1BCCA0);

	MH_CreateHook(CGameStateSetPlayerHook, &hkCGameStateSetPlayer, (LPVOID*)&CGameStateSetPlayerTramp);
	MH_EnableHook(CGameStateSetPlayerHook);

	CRemovePlayerCommandHook = GetCRemovePlayerCommand(GameBase + 0x1650F60);

	MH_CreateHook(CRemovePlayerCommandHook, &hkCRemovePlayerCommand, (LPVOID*)&CRemovePlayerCommandTramp);
	MH_EnableHook(CRemovePlayerCommandHook);

	//CChatMessageHook = GetCChatMessage(GameBase + 0x1E939A0);
	//MH_CreateHook(CChatMessageHook, &hkChatMessage, (LPVOID*)&CChatMessageTramp);
	//MH_EnableHook(CChatMessageHook);

	CCreateEquipmentFunc = GetCCreateEquipmentVariant(GameBase + 0x170C870);
	MH_CreateHook(CCreateEquipmentFunc, &hkCCreateEquipment, (LPVOID*)&CCreateEquipmentTramp);
	MH_EnableHook(CCreateEquipmentFunc);

	AntiBanFunc = GetAntiBan(GameBase + 0x1F97540);
	MH_CreateHook(AntiBanFunc, &hkAntiBan, (LPVOID*)&AntiBanTramp);
	MH_EnableHook(AntiBanFunc);

	//CNameChangeFunc = GetCNameChangeCommand(GameBase + 0x104F70);
	CCrashFunc = GetCCrash(GameBase + 0x1650FF0);
	MH_CreateHook(CCrashFunc, &hkCrash, (LPVOID*)&CCrashTramp);
	MH_EnableHook(CCrashFunc);

	CStartGameCommandFunc = GetCStartGameCommand(GameBase + 0x12724F0);
	MH_CreateHook(CStartGameCommandFunc, &hkStartGame, (LPVOID*)&CStartGameCommandTramp);
	MH_EnableHook(CStartGameCommandFunc);
	GetCCommandFunc = GetCCommand(GameBase + 0x2112854);
	//CNameChangeFunc = GetCNameChangeCommand(GameBase + 0x170C430);
	AIEnableFunc = GetEnableAI(GameBase + 0xBD4EB0);
	ChatLeaveFunc = GetCChatLeaveFake(GameBase + 0x109AED0);
	CPauseGameFunc = GetCPauseGameCommand(GameBase + 0xC2EEC0);


	//Dont be mistaken these are different addresses!
	IncreaseSpeedFunc = GetCGameSpeed(GameBase + 0xD121F0);
	DecreaseSpeedFunc = GetCGameSpeed(GameBase + 0xD12120);

	SetSpeedFunc = GetCGameSetSpeed(GameBase + 0xD12240);

	GCDMF = GetCustomDiffM(GameBase + 0x1272280);
	MH_CreateHook(GCDMF, &hkCustomDiffM, (LPVOID*)&GCDMH);
	MH_EnableHook(GCDMF);

	GetCustomF = GetCustomDiff(GameBase + 0x12722F0);
	MH_CreateHook(GetCustomF, &hkCustomDiff, (LPVOID*)&GetCustomH);
	MH_EnableHook(GetCustomF);

	/*MH_CreateHook(CNameChangeFunc, &hkNameChange, (LPVOID*)&CNameChangeTramp);
	MH_EnableHook(CNameChangeFunc);*/
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)& oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	HookFunctions();
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		break;
	}
	return TRUE;
}
