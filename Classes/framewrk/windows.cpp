#if 0
#define _IN_MAIN
#include "frm_int.hpp"


int					g_SettingsFlg;
int					g_ReqKeyFlg;		// is a hardware key being requested?
short			   *g_ReqKeyPtr;		// which key is being requested?
short				g_CurKey;
int					g_KeyTimeOut;		// tijdelijk werkt options key niet



extern         void framework_EventHandle(int event, int param);
extern HEARTBEAT_FN framework_InitGame(Cvideo *video, Caudio *audio, Ctimer *timer, Cmovie *movie);
extern         void framework_ExitGame(void);


unsigned short *SettingsPic;
unsigned short *ButPic[10];
unsigned short *SwitchPic[6];
unsigned short *SpeakerPic[2];
unsigned short *LoadingPic;
unsigned short *TempPic;



extern short g_RenderMode;

struct KeyConfig
{
	short LeftKey;
	short RightKey;
	short UpKey;
	short DownKey;
	short OptionsKey;
	short FireKey;
	short EscKey;
	short RenderMode;
	short SoundFlg;
};

KeyConfig g_KeyConfig;


#define MOUSETRESHOLD (32)
int g_MouseFlg;
int g_MouseXDown;
int g_MouseYDown;
int g_MouseXCurrent;
int g_MouseYCurrent;

int g_CurDeltaX;

int g_LastSpdX;
int g_LastSpdY;

int frmwrk_CenterX;
int frmwrk_CenterY;





void LoadSettingsPics(void)
{
	SettingsPic  = LoadTGA("settings.tga.gz");
	ButPic[0]	 = LoadTGA("butleft.tga.gz");
	ButPic[1]	 = LoadTGA("butright.tga.gz");
	ButPic[2]	 = LoadTGA("butup.tga.gz");
	ButPic[3]	 = LoadTGA("butdown.tga.gz");
	ButPic[4]	 = LoadTGA("buta.tga.gz");
	ButPic[5]	 = LoadTGA("butb.tga.gz");
	ButPic[6]	 = LoadTGA("butc.tga.gz");
	ButPic[7]	 = LoadTGA("butd.tga.gz");
	ButPic[8]	 = LoadTGA("bute.tga.gz");
	ButPic[9]	 = LoadTGA("butq.tga.gz");
	SwitchPic[0] = LoadTGA("switch1.tga.gz");
	SwitchPic[1] = LoadTGA("switch2.tga.gz");
	SpeakerPic[0] = LoadTGA("speakeroff.tga.gz");
	SpeakerPic[1] = LoadTGA("speakeron.tga.gz");
//	LoadingPic   = LoadTGA("loading.tga.gz");
}



void BlitSettingsElement(unsigned short *Src, int PosX, int PosY, int Width, int Height)
{
	int x,y;

	for(y=0; y<Height; y++)
	{
		for(x=1; x<Width; x++)
		{
			unsigned short Pix;
			Pix = Src[(Width-x) + y*Width];
			if(Pix != 0xf81f)			//transparant?
			{
				SettingsPic[(320-(PosX+x)) + (PosY+y)*320] = Pix;
			}
		}
	}
}

void LoadConfig(void)
{
	//defaults
#if 0
	g_KeyConfig.FireKey		= g_gxkl.vkStart;
	g_KeyConfig.OptionsKey	= g_gxkl.vkB;
	g_KeyConfig.EscKey		= g_gxkl.vkA;
	g_KeyConfig.LeftKey		= g_gxkl.vkDown;
	g_KeyConfig.RightKey	= g_gxkl.vkUp;
	g_KeyConfig.UpKey		= g_gxkl.vkLeft;
	g_KeyConfig.DownKey		= g_gxkl.vkRight;
	g_KeyConfig.RenderMode  = 4;
#endif

	g_KeyConfig.FireKey		= g_KeyList.vkStart;
	g_KeyConfig.OptionsKey	= g_KeyList.vkB;
	g_KeyConfig.EscKey		= g_KeyList.vkA;
	g_KeyConfig.LeftKey		= g_KeyList.vkRight;
	g_KeyConfig.RightKey	= g_KeyList.vkLeft;
	g_KeyConfig.UpKey		= g_KeyList.vkDown;
	g_KeyConfig.DownKey		= g_KeyList.vkUp;
	g_KeyConfig.RenderMode  = 6;    //interpolation + frameskip
	g_KeyConfig.SoundFlg    = 1; 

	FILE *fp;
	fp = fopen( FullPath2("settings.dat"), "rb" );
	if(fp)
	{
		fread( &g_KeyConfig, 1, sizeof(KeyConfig), fp );
		fclose(fp);
	}
	
	g_RenderMode = g_KeyConfig.RenderMode;
}

void SaveConfig(void)
{
	FILE *fp;
	g_KeyConfig.RenderMode = g_RenderMode;

	fp = fopen( FullPath2("settings.dat"), "wb" );
	if(fp)
	{
		fwrite( &g_KeyConfig, 1, sizeof(KeyConfig), fp );
		fclose(fp);
	}
}

int TransformKey(short KeyCode)
{
	if(g_KeyList.vkLeft  == KeyCode)return 1;
	if(g_KeyList.vkUp    == KeyCode)return 3;
	if(g_KeyList.vkRight == KeyCode)return 0;
	if(g_KeyList.vkDown  == KeyCode)return 2;
	if(g_KeyList.vkA     == KeyCode)return 4;
	if(g_KeyList.vkB     == KeyCode)return 5;
	if(g_KeyList.vkC     == KeyCode)return 6;
	if(g_KeyList.vkStart == KeyCode)return 7;
/*	if(g_KeyList.vkLeft  == KeyCode)return 2;
	if(g_KeyList.vkRight == KeyCode)return 3;
	if(g_KeyList.vkUp    == KeyCode)return 1;
	if(g_KeyList.vkDown  == KeyCode)return 0;
	if(g_KeyList.vkA     == KeyCode)return 4;
	if(g_KeyList.vkB     == KeyCode)return 5;
	if(g_KeyList.vkC     == KeyCode)return 6;
	if(g_KeyList.vkStart == KeyCode)return 7;
*/	return 9;
}

void BlitConfig(void)
{
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.FireKey)],    74, 35, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.OptionsKey)], 74, 81, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.EscKey)],    74, 126, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.UpKey)],      202,50, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.DownKey)],    202,99, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.RightKey)],   169,75, 34, 34);
	BlitSettingsElement(ButPic[TransformKey(g_KeyConfig.LeftKey)],    234,75, 34, 34);

	BlitSettingsElement(SpeakerPic[g_KeyConfig.SoundFlg],   320-55 ,138, 41, 36); //(14)


	int schakelaar;
	if(g_RenderMode&4)
	{
		schakelaar = 0;
	}
	else
	{
		schakelaar = 1;
	}
	BlitSettingsElement(SwitchPic[schakelaar], 72, 188, 29, 36);

	if(g_RenderMode&1)
	{
		schakelaar = 0;
	}
	else
	{
		schakelaar = 1;
	}
	BlitSettingsElement(SwitchPic[schakelaar], 145,188, 29, 36);

	if(g_RenderMode&2)
	{
		schakelaar = 1;
	}
	else
	{
		schakelaar = 0;
	}
	BlitSettingsElement(SwitchPic[schakelaar], 218,188, 29, 36);
}


//extern unsigned short levelloadedflg;
unsigned short levelloadedflg;   //voor nu.... moet weer extern worden. deze flag zit IN moonchild

int WINAPI WinMain(	HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPTSTR    lpCmdLine,
				   int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	
	
	lvideo = new Cvideo();
	
	
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	LoadSettingsPics();
	LoadConfig();
	BlitConfig();

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TTT);
	
	laudio = new Caudio();  // create audio AFTER window is created!
	ltimer = new Ctimer();  // Create timer facilities
	lmovie = new Cmovie(laudio);  // Initiate movie playback features
	
	g_SettingsFlg = 0;	//we starten met het settings window
	g_ReqKeyFlg = 0;
	g_KeyTimeOut = 0;

	frmwrk_CenterX = 0;
	frmwrk_CenterY = 0;
	
	gbGameLoop = TRUE;
	
	heartbeat = NULL;

	
	while( 1 )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				return msg.wParam;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else if( gbGameLoop )
		{
			if(g_SettingsFlg)	//settings screen op het beeld?
			{
//				unsigned char buf[4] = {1,2,3,0};
//				int getal;
//				getal = g_KeyConfig.LeftKey;
//				buf[2] = getal/100;
//				getal = getal - ((getal/100)*100);
//				buf[1] = getal/10;
//				buf[0] = getal%10;
				lvideo->DrawSettings();
//				lvideo->DisplayChars2(buf, 5, 5);

			}
			else
			{
				lmovie->movieplay();            // if movie is playing this routine will handle frame advancement
				if (heartbeat != NULL)
				{
					heartbeat = (HEARTBEAT_FN) heartbeat();
					if(heartbeat == NULL)  // No heartbeat anymore, Let's close
					{
						PostMessage ( ghWnd, WM_CLOSE, 0, 0);
					}

					if(g_MouseFlg==1 && levelloadedflg==1)	//alleen tijdens levels!
					{
						int DeltaX,DeltaY;
						int SpdX,SpdY;
						SpdX = SpdY = 0;
						DeltaX = (320-(g_MouseYCurrent)) - (frmwrk_CenterX/2);
						DeltaY = (g_MouseXCurrent) - (frmwrk_CenterY/2);
						if(DeltaX > MOUSETRESHOLD) SpdX = 1;
						if(DeltaX < -MOUSETRESHOLD) SpdX = -1;
						if(DeltaY > MOUSETRESHOLD) SpdY = 1;
						if(DeltaY < -MOUSETRESHOLD) SpdY = -1;

						g_CurDeltaX = DeltaX;
						if(SpdX != g_LastSpdX)
						{
							switch(SpdX)
							{
							case -1:
								framework_EventHandle(FW_KEYUP,(int) 39);		//rechts
								framework_EventHandle(FW_KEYDOWN,(int) 37);		//links
								break;
							case 0:
								framework_EventHandle(FW_KEYUP,(int) 39);		//rechts
								framework_EventHandle(FW_KEYUP,(int) 37);		//links
								break;
							case 1:
								framework_EventHandle(FW_KEYUP,(int) 37);		//links
								framework_EventHandle(FW_KEYDOWN,(int) 39);		//rechts
								break;
							}
						}
						if(SpdY != g_LastSpdY)
						{
							switch(SpdY)
							{
							case -1:
								framework_EventHandle(FW_KEYUP,(int) 40);		//down
								framework_EventHandle(FW_KEYDOWN,(int) 38);		//up
								break;
							case 0:
								framework_EventHandle(FW_KEYUP,(int) 38);		//up
								framework_EventHandle(FW_KEYUP,(int) 40);		//down
								break;
							case 1:
								framework_EventHandle(FW_KEYUP,(int) 38);		//up
								framework_EventHandle(FW_KEYDOWN,(int) 40);		//down
								break;
							}
						}
						g_LastSpdX = SpdX;
						g_LastSpdY = SpdY;
					}
					else
					{
						if(g_LastSpdX != 0)
						{
							framework_EventHandle(FW_KEYUP,(int) 39);   //rechts
							framework_EventHandle(FW_KEYUP,(int) 37); //links
						}
						if(g_LastSpdY != 0)
						{
							framework_EventHandle(FW_KEYUP,(int) 38); //up
							framework_EventHandle(FW_KEYUP,(int) 40); //down
						}
						g_LastSpdX = 0;
						g_LastSpdY = 0;
					}

				}
			}
		}
	}
	
	return msg.wParam;
}




	switch (message) 
	{
		case WM_LBUTTONDOWN:
			g_MouseFlg = 1;
			g_MouseXDown = LOWORD(lParam);
			g_MouseYDown = HIWORD(lParam);
			g_MouseXCurrent = LOWORD(lParam);
			g_MouseYCurrent = HIWORD(lParam);
			if(g_SettingsFlg)
			{
// check on which key we press
				if(g_MouseXDown>75&&g_MouseXDown<109 && g_MouseYDown>169&&g_MouseYDown<213)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.RightKey;
					g_KeyConfig.RightKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>75&&g_MouseXDown<109 && g_MouseYDown>234&&g_MouseYDown<268)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.LeftKey;
					g_KeyConfig.LeftKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>50&&g_MouseXDown<84 && g_MouseYDown>202&&g_MouseYDown<236)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.UpKey;
					g_KeyConfig.UpKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>99&&g_MouseXDown<133 && g_MouseYDown>202&&g_MouseYDown<236)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.DownKey;
					g_KeyConfig.DownKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>35&&g_MouseXDown<69 && g_MouseYDown>74&&g_MouseYDown<108)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.FireKey;
					g_KeyConfig.FireKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>81&&g_MouseXDown<115 && g_MouseYDown>74&&g_MouseYDown<108)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.OptionsKey;
					g_KeyConfig.OptionsKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>126&&g_MouseXDown<160 && g_MouseYDown>74&&g_MouseYDown<108)
				{
					g_ReqKeyFlg=1;
					g_ReqKeyPtr = &g_KeyConfig.EscKey;
					g_KeyConfig.EscKey = 254;
					BlitConfig();
				}
				if(g_MouseXDown>00&&g_MouseXDown<22 && g_MouseYDown>0&&g_MouseYDown<40)
				{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				if(g_MouseXDown>00&&g_MouseXDown<22 && g_MouseYDown>40&&g_MouseYDown<110)
				{
					g_MouseFlg = 0;
					g_SettingsFlg = 0;
				}
				if(g_MouseXDown>188&&g_MouseXDown<224 && g_MouseYDown>72&&g_MouseYDown<101)
				{
					g_RenderMode^=4;
					BlitConfig();
				}
				if(g_MouseXDown>188&&g_MouseXDown<224 && g_MouseYDown>145&&g_MouseYDown<174)
				{
					g_RenderMode^=1;
					BlitConfig();
				}
				if(g_MouseXDown>188&&g_MouseXDown<224 && g_MouseYDown>218&&g_MouseYDown<247)
				{
					g_RenderMode^=2;
					BlitConfig();
				}
				if(g_MouseXDown>138&&g_MouseXDown<174 && g_MouseYDown>(320-55)&&g_MouseYDown<(320-14))
				{
					if(g_KeyConfig.SoundFlg)g_KeyConfig.SoundFlg=0; else g_KeyConfig.SoundFlg=1;
					BlitConfig();
				}
			}
			break;
		case WM_MOUSEMOVE:
			g_MouseXCurrent = LOWORD(lParam);
			g_MouseYCurrent = HIWORD(lParam);
			break;
		case WM_LBUTTONUP:
			g_MouseFlg = 0;
			break;
		case WM_USER+1000:
			{
#if 1 //$$$
			  HDC dc;

			  dc = GetDC(hWnd);

	//		  ShowCursor(FALSE);
			  if ( !lvideo->on(dc, 640, 480, 256) )
			  {
				PostMessage ( hWnd, WM_CLOSE, 0, 0);        
				break;
			  }
  
			  heartbeat = framework_InitGame(lvideo, laudio, ltimer, lmovie);

			  if (heartbeat == NULL)
			  {
				PostMessage ( hWnd, WM_CLOSE, 0, 0);
			  }
#endif
			}
		  break;
		case WM_DESTROY:
			CommandBar_Destroy(hwndCB);
#if 0 //$$$
		  gbGameLoop = FALSE;
		  framework_ExitGame();
		  lvideo->off();
		  log_out("Application exit");
		  log_exit();     // stop logging

		  delete (lmovie);
		  delete (ltimer);
		  delete (laudio);
		  delete (lvideo);
#endif
		    SaveConfig();
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			vkKey = (short)wParam;
			g_CurKey = vkKey;

			if(g_SettingsFlg)
			{
				if(g_ReqKeyFlg)
				{
					if(vkKey == g_KeyList.vkLeft || vkKey == g_KeyList.vkRight || vkKey == g_KeyList.vkUp || vkKey == g_KeyList.vkDown || vkKey == g_KeyList.vkA || vkKey == g_KeyList.vkB || vkKey == g_KeyList.vkC || vkKey == g_KeyList.vkStart)
					{
						g_ReqKeyFlg = 0;
						*g_ReqKeyPtr = vkKey;
						BlitConfig();
						g_CurKey =255;
						g_KeyTimeOut=64;
						break;
					}
				}
				else
				{
					if(vkKey == g_KeyConfig.OptionsKey)
					{
						g_SettingsFlg = 0;
					}
				}
			}
			else
			{
#if 1 //$$$
				if (vkKey == g_KeyConfig.RightKey) {
	//				ClearScreen(0,0,0); // Black

					framework_EventHandle(FW_KEYDOWN,(int) 39);   //rechts

					break;
				}
				if (vkKey == g_KeyConfig.LeftKey) {
	//				ClearScreen(255,0,0); // Red
					framework_EventHandle(FW_KEYDOWN,(int) 37); //links
					break;
				}

				if (vkKey == g_KeyConfig.UpKey) {
					framework_EventHandle(FW_KEYDOWN,(int) 38);	//up
					break;
				}

				if (vkKey == g_KeyConfig.DownKey) {
					framework_EventHandle(FW_KEYDOWN,(int) 40);	//down
					break;
				}
				if (vkKey == g_KeyConfig.FireKey) {
					framework_EventHandle(FW_KEYDOWN,(int) 32);
					break;
				}
				if (vkKey == g_KeyConfig.EscKey) {
					framework_EventHandle(FW_KEYDOWN,(int) VK_ESCAPE);
					break;
				}
				if (vkKey == g_KeyConfig.OptionsKey) {
					g_SettingsFlg = 1;
					g_CurKey = 255;
	//				extern short g_RenderMode;
	//				g_RenderMode++;
	//				g_RenderMode&=7;
				}
//				if (vkKey == g_KeyConfig.EscKey) {
//					break;
//				}
			}
#endif
			return DefWindowProc(hWnd, message, wParam, lParam);
		break;
#if 1  //$$$
		case WM_KEYUP:
			vkKey = (short)wParam;

			if(g_SettingsFlg)
			{
				g_CurKey = 255;
			}
			else
			{
				if (vkKey == g_KeyConfig.RightKey) {
					framework_EventHandle(FW_KEYUP,(int) 39);
					break;
				}
				if (vkKey == g_KeyConfig.LeftKey) {
					framework_EventHandle(FW_KEYUP,(int) 37);
					break;
				}
				if (vkKey == g_KeyConfig.UpKey) {
					framework_EventHandle(FW_KEYUP,(int) 38);
					break;
				}
				if (vkKey == g_KeyConfig.DownKey) {
					framework_EventHandle(FW_KEYUP,(int) 40);
					break;
				}
				if (vkKey == g_KeyConfig.FireKey) {
	//				ClearScreen(0,0,0); // Black

					framework_EventHandle(FW_KEYUP,(int) 32);

					break;
				}
				if (vkKey == g_KeyConfig.EscKey) {
					framework_EventHandle(FW_KEYUP,(int) VK_ESCAPE);
					break;
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		break;
#endif
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}



#endif

