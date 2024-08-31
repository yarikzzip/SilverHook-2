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


typedef CNameChange* (__fastcall* GetCNameChangeCommand)(void* pThis, void* name);
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
GetCPauseGameCommand CPauseGameTramp;

typedef CAiEnableCommand* (__fastcall* GetEnableAI)(void* pThis, int* tag, int toggled);
GetEnableAI AIEnableFunc;

typedef CChatLeaveFake* (_fastcall* GetCChatLeaveFake)(void* pThis, int mID);
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

typedef EmptyTest* (__fastcall* GetJoinCommand)(void* pThis, CString* Name, int nID, __int64 a4);
GetJoinCommand JoinHook;
GetJoinCommand JoinTramp;

typedef EmptyTest* (__fastcall* GetAutoSave)(void* pThis, bool Send);
GetAutoSave AutoSaveFunc;

typedef EmptyTest* (__fastcall* GetAntiKick)();
GetAntiKick AntiKickFunc;
GetAntiKick AntiKickTramp;


typedef EmptyTest* (__fastcall* GetAntiBan)();
GetAntiBan AntiBanFunc;
GetAntiBan AntiBanTramp;


typedef CSessionConfig* (__fastcall* GetSessionConfig)(void* pThis, __int64 NetAddress, void* GameName, int Type, int BasicType, int Status, __int64 Version, bool isResetAllowed);
GetSessionConfig SessionConfigHook;
GetSessionConfig SessionConfigTramp;

typedef CMultiplayerConfig* (__fastcall* GetMultiplayerConfig)(void* pThis, void* Name, DWORD* a3, void* SessionConfig, bool IsGameOwner);
GetMultiplayerConfig GameOwnerHook;
GetMultiplayerConfig GameOwnerTramp;

typedef void* (__fastcall* GetSendMessage)(void* pThis, void* Message, void* Buffer);
GetSendMessage CSendMessage;
GetSendMessage CSendMessageHook;

typedef void* (__fastcall* GetMessageValue)(void* pThis, void* Message, void* User);
GetMessageValue CMessageValueFunc;
GetMessageValue CMessageValueHook;



bool bGameOwner = false;
bool bGameReturn = false;

bool bSetSpeed = false;

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

bool bGhost2 = false;
bool bGhost3 = false;

//RemovePlayerCommand
void* pCRemovePlayer = nullptr;
ERemovalReason dReason;
int RMID;
__int64 dRUnknown;

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

	uintptr_t address = GameBase + 0x185CEFF;

	unsigned char patch[] = { 0x74, 0x0F, 0x83, 0xFA, 0x01, 0x00 }; 

	void* MLHmem = VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (MLHmem != nullptr)
	{
		DWORD relativeOffset = (GameBase + 0x185CF10) - (address + sizeof(patch));
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

void* x = operator new(38);
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

	if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		StartGameFunc();

		

	if (GetAsyncKeyState(VK_SUBTRACT) & 1) {
		CGameSpeed* DecreaseSpeed = (CGameSpeed*)GetCCommandFunc(80);
		DecreaseSpeed = DecreaseSpeedFunc(DecreaseSpeed);
		CSessionPostTramp(pCSession, DecreaseSpeed, true);
	}
		

	if (GetAsyncKeyState(VK_ATTN) & 1) {
		CGameSpeed* IncreaseSpeed = (CGameSpeed*)GetCCommandFunc(48);
		IncreaseSpeed = IncreaseSpeedFunc(IncreaseSpeed);
		CSessionPostTramp(pCSession, IncreaseSpeed, true);
	}



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
			ImGui::Checkbox("Remove Name", &bJoinAsGhost);
			ImGui::Checkbox("Multiplayer Lobby Hack", &bCE);	
			ImGui::Checkbox("FakeSpammer", &FakeSpammer);
			ImGui::Checkbox("Join as Ghost", &bGhost2);
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
				//CString* tN = dUser;
				//CString* tNN = dHname;
				//CString* empty = new CString;

				unsigned __int64* HOIName = (unsigned __int64*)SizeF(32ui64);
				*HOIName = 0i64;
				HOIName[2] = 0i64;
				HOIName[3] = 15i64;

				unsigned __int64* SteamName = (unsigned __int64*)SizeF(32ui64);
				*SteamName = 0i64;
				SteamName[2] = 0i64;
				SteamName[3] = 15i64;


				CAddPlayerCommand* AddFake = (CAddPlayerCommand*)GetCCommandFunc(200);

				FakeM2++;

				memcpy(HOIName, "SilverHook", 10);
				memcpy(SteamName, "RSilverXK 4on YT" ,18);
				AddFake = CAddPlayerCommandTramp(AddFake, (CString*)HOIName, (CString*)SteamName, tt, FakeM2, false, tP);


				CSessionPostTramp(pCSession, AddFake, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Fake= leave", ImVec2(140, 28)) && pCSession != nullptr) {
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
			ImGui::NextColumn();
			if (ImGui::Button("Disable DLCs", ImVec2(140, 28)) && pCSession != nullptr) {
				Crasher(0);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Enable DLCs", ImVec2(140, 28)) && pCSession != nullptr) {
				Crasher(4095);
			}
			if (bGhost3) {
				CRemovePlayerCommand* RemovePlayer = (CRemovePlayerCommand*)GetCCommandFunc(200);
				RemovePlayer = CRemovePlayerCommandTramp(RemovePlayer, dMachine, 0, dRUnknown);
				CSessionPostTramp(pCSession, RemovePlayer, true);
				bGhost3 = false;
			}
			/*(ImGui::NextColumn();
			if (ImGui::Button("Fake Leave", ImVec2(140, 28)) && pCSession != nullptr) {
				CChatLeaveFake* ChatLeave = (CChatLeaveFake*)GetCCommandFunc(56);
				ChatLeave = ChatLeaveFunc(ChatLeave, iMyMachineID);
				CSessionPostTramp(pCSession, ChatLeave, true);
			}
			ImGui::NextColumn();
			if (ImGui::Button("Fake Leave All", ImVec2(140, 28)) && pCSession != nullptr) {
				CChatLeaveFake* ChatLeave = (CChatLeaveFake*)GetCCommandFunc(56);
				for (int i = 1; i < iMyMachineID; i++) {
					ChatLeave = ChatLeaveFunc(ChatLeave, i);
					CSessionPostTramp(pCSession, ChatLeave, true);
				}
			}*/
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
			ImGui::NextColumn();
			if (ImGui::Button("AutoSave", ImVec2(140, 28)) && pCSession != nullptr) {
				
				EmptyTest* AutoSave = (EmptyTest*)GetCCommandFunc(56);
				AutoSave = AutoSaveFunc(AutoSave, 1);

				CSessionPostTramp(pCSession, AutoSave, true);
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
					int* pTag = &Tag;
					CGameStateSetPlayerTramp(pCGameState, pTag);

					//ChangeIntAddressValue(0x2C97110, 0x4B0, Tag);
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
			ImGui::Checkbox("GameOwner", &bGameOwner);
			//ImGui::Checkbox("SetSpeed", &bSetSpeed);
			ImGui::Text("Enabling debug WILL bring up a console\n as a seperate window. \nDO NOT CLOSE IT!");
			if (ImGui::Button("Send") && pCSession != nullptr)
			{
				

			}
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
		
		
		if (bSetSpeed) {
			std::string sTagBuffer = TagBuffer;
			int speed = std::stoi(TagBuffer);

			if (sTagBuffer.length() > 0)
			{
				CSetSpeed* Speed = (CSetSpeed*)GetCCommandFunc(56);
				Speed = SetSpeedFunc(Speed, speed);
				CSessionPostTramp(pCSession, Speed, true);
			}
		}
	


		if (Debug) {
			if (!AllocatedConsole) {
				AllocConsole();
				AllocatedConsole = true;
			}
		}
		
		
		if (bCrasher) {
			Crasher(0);
			EmptyTest* AutoSave = (EmptyTest*)GetCCommandFunc(56);
			AutoSave = AutoSaveFunc(AutoSave, 1);
			CSessionPostTramp(pCSession, AutoSave, true);
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
			CAddPlayerCommand* AddFake = (CAddPlayerCommand*)GetCCommandFunc(200);
			FakeM++;
			(CString*)memcpy(a1, "SilverXK", 8);
			(CString*)memcpy(a2, "discord/k2EBbsQf69", 18);
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


EmptyTest* __fastcall hkAntiBan() {

	printm("Ban Called");
	CRemovePlayerCommand* RemovePlayer = (CRemovePlayerCommand*)GetCCommandFunc(200);
	RemovePlayer = CRemovePlayerCommandTramp(RemovePlayer, dMachine, 3, dRUnknown);
	
	if (bRefuseConnect)
		return (EmptyTest*)CSessionPostTramp(pCSession, RemovePlayer, true);
	if(!bRefuseConnect)
		return AntiBanTramp();
}

EmptyTest* __fastcall hkAntiKick() {

	printm("Kick Called");
	CRemovePlayerCommand* RemovePlayer = (CRemovePlayerCommand*)GetCCommandFunc(200);
	RemovePlayer = CRemovePlayerCommandTramp(RemovePlayer, dMachine, 2, dRUnknown);
	if (bRefuseConnect)
		return (EmptyTest*)CSessionPostTramp(pCSession, RemovePlayer, true);
	if (!bRefuseConnect)
		return AntiKickTramp();
}



bool __fastcall hkCSessionPost(void* pThis, CCommand* pCommand, bool ForceSend)
{
	
	pCSession = pThis;
	if (Debug) {
		printm("CSP Called");
	}
	return CSessionPostTramp(pThis, pCommand, ForceSend);
}

CAddPlayerCommand* __fastcall hkCAddPlayerCommand(CAddPlayerCommand* pThis, CString* User, CString* Name, DWORD* unknown, int nMachineId, bool bHotjoin, __int64 a7)
{
	//CAddPlayerCommand* cmd = static_cast<CAddPlayerCommand*>(pThis);

	//CString* tttt = new CString;
	

	//CString* Test = (CString*)(pThis->_User);


	//memcpy(Test, "Test", 4);

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

		//User->_str = "Test";

		//CNameChangeFunc(&pThis->_User, User);
		//if (Debug)
			//printm("Name Called");
	}
	
	pCAddPlayer = pThis;
	dUser = (CString*)User;
	dHname = (CString*)Name;
	dUnknown = unknown;
	dMachine = nMachineId;
	dPdx = a7;
	//if (bGhost2)
		//bGhost3 = true;
	int i = 0;
	int* TagPtr = &i;
	CAiEnableCommand* EnableAI = (CAiEnableCommand*)GetCCommandFunc(56);
	 
	
	if (Debug)
		printm("APC Called");

	if(!bGhost2)
		return CAddPlayerCommandTramp(pThis, (CString*)User, (CString*)Name, unknown, nMachineId, bHotjoin, a7);

	if(bGhost2)
		return (CAddPlayerCommand * )AIEnableFunc(EnableAI, TagPtr, 0);
}

CRemovePlayerCommand* __fastcall hkCRemovePlayerCommand(void* pThis, int _machineID, ERemovalReason eReason, __int64 a4)
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

CPauseGameCommand* __fastcall hkPauseGame(void* pThis, __int64 a2, char a3, char a4) {
	if (Debug) {
		printm("hkPauseGame Called");
		std::string x = std::to_string(a2);
		printm(x);
		x = std::to_string(a3);
		printm(x);
		x = std::to_string(a4);
		printm(x);
	}
	return CPauseGameTramp(pThis, a2, a3);
}

EmptyTest* __fastcall hkJoinCommand(void* pThis, CString* Name, int nID, __int64 a4) {

	if (Debug) {
		std::string x = std::to_string(nID);
		printm(x);
		printm((char*)Name);
	}

	
		return JoinTramp(pThis, Name, nID, a4);
	
	
}


CSessionConfig* __fastcall hkSessionConfig(void* pThis, __int64 NetAddress, void* GameName, int Type, int BasicType, int Status, __int64 Version, bool isResetAllowed) {




	if (bGameOwner) {
		isResetAllowed = 1;
		Type = 2;
		//BasicType = 1;
		//Status = 1;
	}

	if (Debug) {
		printm("SessionConfig Called");
		printm(std::to_string(Type));
		printm(std::to_string(BasicType));
		printm(std::to_string(Status));
		printm(std::to_string(Version));
		printm(std::to_string(isResetAllowed));
	}

	


	return SessionConfigTramp(pThis, NetAddress, GameName, Type, BasicType, Status, Version, isResetAllowed);
}


CMultiplayerConfig* __fastcall hkGameOwner(void* pThis, void* Name, DWORD* a2, void* SessionConfig, bool IsGameOwner) {

	if (bGameOwner)
		IsGameOwner = 1;



	if (Debug) {
		printm("GameOwner Called");
		printm(std::to_string(IsGameOwner));
	}

	
	return GameOwnerTramp(pThis, Name, a2, SessionConfig, IsGameOwner);
}

void* __fastcall hkSendMessage(void* pThis, CString* Message, void* pBuffer) {


	if(Debug){
		printm("SendMessageCalled");
		//std::to_string(Message);
		printm((char*)Message);
		printm((char*)pBuffer);
	}



	return CSendMessage(pThis, Message, pBuffer);
}



void HookFunctions() {

	CSessionPostHook = CSessionPost(GameBase + 0x1DE2CC0);
	MH_CreateHook(CSessionPostHook, &hkCSessionPost, (LPVOID*)&CSessionPostTramp);
	MH_EnableHook(CSessionPostHook);

	CAddPlayerCommandHook = GetCAddPlayerCommand(GameBase + 0x1651050);
	MH_CreateHook(CAddPlayerCommandHook, &hkCAddPlayerCommand, (LPVOID*)&CAddPlayerCommandTramp);
	MH_EnableHook(CAddPlayerCommandHook);
	
	AutoSaveFunc = GetAutoSave(GameBase + 0x0C2EDE0);

	CGameStateSetPlayerHook = CGameStateSetPlayer(GameBase + 0x1BCCA0);

	MH_CreateHook(CGameStateSetPlayerHook, &hkCGameStateSetPlayer, (LPVOID*)&CGameStateSetPlayerTramp);
	MH_EnableHook(CGameStateSetPlayerHook);

	CRemovePlayerCommandHook = GetCRemovePlayerCommand(GameBase + 0x1651130);

	MH_CreateHook(CRemovePlayerCommandHook, &hkCRemovePlayerCommand, (LPVOID*)&CRemovePlayerCommandTramp);
	MH_EnableHook(CRemovePlayerCommandHook);

	//SessionConfigHook = GetSessionConfig(GameBase + 0x1E7E990);
	//MH_CreateHook(SessionConfigHook, &hkSessionConfig, (LPVOID*)&SessionConfigTramp);
	//MH_EnableHook(SessionConfigHook);

	//GameOwnerHook = GetMultiplayerConfig(GameBase + 0x16502F0);
	//MH_CreateHook(GameOwnerHook, &hkGameOwner, (LPVOID*)&GameOwnerTramp);
	//MH_EnableHook(GameOwnerHook);



	//CSendMessageHook = GetSendMessage(GameBase + 0x1E939A0);
	//MH_CreateHook(CSendMessageHook, &hkSendMessage, (LPVOID*)&CSendMessage);
	//MH_EnableHook(CSendMessageHook);

	//CMessageValueFunc = GetMessageValue(GameBase + 0x1E90D20);

	//CChatMessageHook = GetCChatMessage(GameBase + 0x1E939A0);
	//MH_CreateHook(CChatMessageHook, &hkChatMessage, (LPVOID*)&CChatMessageTramp);
	//MH_EnableHook(CChatMessageHook);

	CCreateEquipmentFunc = GetCCreateEquipmentVariant(GameBase + 0x170CA40);
	MH_CreateHook(CCreateEquipmentFunc, &hkCCreateEquipment, (LPVOID*)&CCreateEquipmentTramp);
	MH_EnableHook(CCreateEquipmentFunc);

	//AntiBanFunc = GetAntiBan(GameBase + 0x1DE1610);
	//AntiBanFunc = GetAntiBan(GameBase + 0x1F97540);
	//MH_CreateHook(AntiBanFunc, &hkAntiBan, (LPVOID*)&AntiBanTramp);
	//MH_EnableHook(AntiBanFunc);

	//AntiKickFunc = GetAntiKick(GameBase + 0x1DE29B0);
	//MH_CreateHook(AntiKickFunc, &hkAntiKick, (LPVOID*)&AntiKickTramp);
	//MH_EnableHook(AntiKickFunc);

	//CNameChangeFunc = GetCNameChangeCommand(GameBase + 0x104F70);
	CCrashFunc = GetCCrash(GameBase + 0x16511C0);
	MH_CreateHook(CCrashFunc, &hkCrash, (LPVOID*)&CCrashTramp);
	MH_EnableHook(CCrashFunc);



	//JoinHook = GetJoinCommand(GameBase + 0x109ADE0);
	//MH_CreateHook(JoinHook, &hkJoinCommand, (LPVOID*)&JoinTramp);
	//MH_EnableHook(JoinHook);
		


	CStartGameCommandFunc = GetCStartGameCommand(GameBase + 0x1272680);
	MH_CreateHook(CStartGameCommandFunc, &hkStartGame, (LPVOID*)&CStartGameCommandTramp);
	MH_EnableHook(CStartGameCommandFunc);
	GetCCommandFunc = GetCCommand(GameBase + 0x2112A24);
	
	AIEnableFunc = GetEnableAI(GameBase + 0xBD5040);
	//ChatLeaveFunc = GetCChatLeaveFake(GameBase + 0x109AED0);

	CPauseGameFunc = GetCPauseGameCommand(GameBase + 0xC2F050);
	MH_CreateHook(CPauseGameFunc, &hkPauseGame, (LPVOID*)&CPauseGameTramp);
	MH_EnableHook(CPauseGameFunc);

	//Dont be mistaken these are different addresses!
	IncreaseSpeedFunc = GetCGameSpeed(GameBase + 0xD12380);
	DecreaseSpeedFunc = GetCGameSpeed(GameBase + 0xD12250);

	SetSpeedFunc = GetCGameSetSpeed(GameBase + 0xD123D0);

	GCDMF = GetCustomDiffM(GameBase + 0x1272410);
	MH_CreateHook(GCDMF, &hkCustomDiffM, (LPVOID*)&GCDMH);
	MH_EnableHook(GCDMF);

	//GetCustomF = GetCustomDiff(GameBase + 0x12722F0);
	//MH_CreateHook(GetCustomF, &hkCustomDiff, (LPVOID*)&GetCustomH);
	//MH_EnableHook(GetCustomF);

	CNameChangeFunc = GetCNameChangeCommand(GameBase + 0x0104F70);
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
