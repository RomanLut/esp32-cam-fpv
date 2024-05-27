#include "osd_menu.h"
#include <cmath>
#include "util.h"
#include "osd.h"
#include "main.h"

OSDMenu g_osdMenu;

//=======================================================
//=======================================================
OSDMenu::OSDMenu()
{
    this->visible = false;
}

//=======================================================
//=======================================================
void OSDMenu::drawMenuTitle( const char* caption )
{
    ImVec4 color = (ImVec4)ImColor(97,137,105);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.02f, 0.5f));
    ImGui::Button(caption, ImVec2( this->bWidth, this->bHeight ) );
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    
    this->itemsCount = 0;
    this->keyHandled = false;

    ImGui::Dummy(ImVec2(0.0f, 20.0f));
}

//=======================================================
//=======================================================
void OSDMenu::drawStatus( const char* caption )
{
    ImVec4 color = (ImVec4)ImColor(48,48,48);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.02f, 0.5f));
    ImGui::Button(caption, ImVec2( this->sWidth, this->bHeight ) );
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
}

//=======================================================
//=======================================================
bool OSDMenu::drawMenuItem( const char* caption, int itemIndex, bool clip )
{
    int d = itemIndex - this->selectedItem;
    int d1 = 2;
    int d2 = -2;
    if ( this->selectedItem == 0) d1+=2;
    if ( this->selectedItem == 1) d1+=1;
    if ( clip  && ((d>d1)|| (d<d2)))
    {
        return false;
    }

    bool focused = this->selectedItem == itemIndex;
    bool res = focused && ( ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_RightArrow)) && !this->keyHandled;

    ImGui::Indent();

    ImVec4 color = focused ? (ImVec4)ImColor(77,137,205) : (ImVec4)ImColor(37,51,88);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.02f, 0.5f));
    if ( ImGui::Button(caption, ImVec2( this->bWidth, this->bHeight ) ) )
    {
        res = true;
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::Unindent();

    this->itemsCount = itemIndex + 1;

    this->keyHandled |= res;

    if ( res )
    {
        this->selectedItem = itemIndex;
    }

    return res;
}

//=======================================================
//=======================================================
void OSDMenu::draw(Ground2Air_Config_Packet& config)
{
    if (!this->visible) 
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_MouseRight))
        {
            this->visible = true;
            this->menuId = OSDMenuId::Main;
            this->selectedItem = 0;
            return;
        }
        else
        {
            return;
        }
    }

    int windowWidth = 500;
    int windowHeight = 600;

    this->bWidth = windowWidth - 58;
    this->sWidth = windowWidth;
    this->bHeight = 35;

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowPos(ImVec2(floor( (screenSize.x - windowWidth) / 2) , floor( (screenSize.y - windowHeight) /2 + 120 )), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::Begin("OSD_MENU", NULL, ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoNav |
                            ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoBackground
                            );

    switch (this->menuId)
    {
        case OSDMenuId::Main: this->drawMainMenu(config); break;
        case OSDMenuId::CameraSettings: this->drawCameraSettingsMenu(config); break;
        case OSDMenuId::Resolution: this->drawResolutionMenu(config); break;
        case OSDMenuId::Brightness: this->drawBrightnessMenu(config); break;
        case OSDMenuId::Contrast: this->drawContrastMenu(config); break;
        case OSDMenuId::Exposure: this->drawExposureMenu(config); break;
        case OSDMenuId::Saturation: this->drawSaturationMenu(config); break;
        case OSDMenuId::Sharpness: this->drawSharpnessMenu(config); break;
        case OSDMenuId::ExitToShell: this->drawExitToShellMenu(config); break;
        case OSDMenuId::Letterbox: this->drawLetterboxMenu(config); break;
        case OSDMenuId::WifiRate: this->drawWifiRateMenu(config); break;
        case OSDMenuId::WifiChannel: this->drawWifiChannelMenu(config); break;
        case OSDMenuId::Restart: this->drawRestartMenu(config); break;
        case OSDMenuId::FEC: this->drawFECMenu(config); break;
        case OSDMenuId::GSSettings: this->drawGSSettingsMenu(config); break;
        case OSDMenuId::OSDFont: this->drawOSDFontMenu(config); break;
    }

    if ( ImGui::IsKeyPressed(ImGuiKey_UpArrow) && this->selectedItem > 0 )
    {
        this->selectedItem--;
    }

    if ( ImGui::IsKeyPressed(ImGuiKey_DownArrow) && this->selectedItem < (this->itemsCount - 1) )
    {
        this->selectedItem++;
    }

    ImGui::End();
}

//=======================================================
//=======================================================
void OSDMenu::drawMainMenu(Ground2Air_Config_Packet& config)
{
    {
        char buf[256];
        sprintf( buf, "ESP32-CAM-FPV v%s.%d %s##title0", FW_VERSION, PACKET_VERSION, s_isOV5640 ? "OV5640" : "OV2640");
        this->drawMenuTitle( buf );
    }

    {
        char buf[256];
        sprintf(buf, "Resolution: %s##0", resolutionName[(int)config.camera.resolution]);
        if ( this->drawMenuItem( buf, 0) )
        {
            if ( config.camera.resolution == Resolution::VGA16) this->selectedItem = 0;
            else if ( config.camera.resolution == Resolution::VGA) this->selectedItem = 1;
            else if ( config.camera.resolution == Resolution::SVGA16) this->selectedItem = 2;
            else if ( config.camera.resolution == Resolution::SVGA) this->selectedItem = 3;
            else if ( config.camera.resolution == Resolution::XGA16) this->selectedItem = 4;
            else if ( config.camera.resolution == Resolution::HD) this->selectedItem = 5;
            else selectedItem = 0;
            this->goForward( OSDMenuId::Resolution, selectedItem );
        }
    }
    
    {
        char buf[256];
        sprintf(buf, "Wifi Channel: %d##1", s_groundstation_config.wifi_channel);
        if ( this->drawMenuItem( buf, 1) )
        {
            this->goForward( OSDMenuId::WifiChannel, s_groundstation_config.wifi_channel-1);
        }
    }

    {
        char buf[256];
        int i = config.wifi_rate == WIFI_Rate::RATE_G_18M_ODFM ? 0 : 
            config.wifi_rate == WIFI_Rate::RATE_G_24M_ODFM ? 1 :
            config.wifi_rate == WIFI_Rate::RATE_G_36M_ODFM ? 2 :
            config.wifi_rate == WIFI_Rate::RATE_N_19_5M_MCS2 ? 3 :
            config.wifi_rate == WIFI_Rate::RATE_N_26M_MCS3 ? 4 :
            config.wifi_rate == WIFI_Rate::RATE_N_39M_MCS4 ? 5 : 6;
        const char* rates[] = {"OFDM 18Mbps", "OFDM 24Mbps", "OFDM 36Mbps", "MCS2L 19.5Mbps", "MCS3L 26Mbps", "MCS4L 39Mbps", "Other"};
        sprintf(buf, "Wifi Rate: %s##2", rates[i]);
        if ( this->drawMenuItem( buf, 2) )
        {
            this->goForward( OSDMenuId::WifiRate, i );
        }
    }

    {
        char buf[256];
        int i  = config.fec_codec_n == 8 ? 0 : config.fec_codec_n == 12 ? 2 : 1;
        const char* levels[] = {"Weak (6/8)", "Medium (6/10)", "Strong (6/12)"};
        sprintf(buf, "FEC: %s##3", levels[i]);
        if ( this->drawMenuItem( buf, 3) )
        {
            this->goForward( OSDMenuId::FEC, i );
        }
    }

    if ( this->drawMenuItem( "Camera Settings...", 4) )
    {
        this->goForward( OSDMenuId::CameraSettings, 0 );

        if ( s_isOV5640 && config.camera.vflip )
        {
            config.camera.vflip = false;
            saveGround2AirConfig(config);
        }
    }

    if ( this->drawMenuItem( "Ground Station Settings...", 5) )
    {
        this->goForward( OSDMenuId::GSSettings, 0 );
    }

    //this->drawMenuItem( "OSD...", 5);

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    {
        char buf[256];
        sprintf( buf, "AIR SD: %s%s%s %.2fGB/%.2fGB##status0", 
            s_SDDetected && !s_SDError? "Ok" : "?", s_SDError ? " Error" :"",  s_SDSlow ? " Slow" : "",
            s_SDFreeSpaceGB16 / 16.0f, s_SDTotalSpaceGB16 / 16.0f
        );

        this->drawStatus( buf );
    }

    if ( this->exitKeyPressed())
    {
        this->visible = false;
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawCameraSettingsMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> Camera Settings" );
    
    {
        char buf[256];
        sprintf(buf, "Brightness: %d##0", config.camera.brightness);
        if ( this->drawMenuItem( buf, 0) )
        {
            this->goForward( OSDMenuId::Brightness, config.camera.brightness + 2 );
        }
    }

    {
        char buf[256];
        sprintf(buf, "Contrast: %d##1", config.camera.contrast);
        if ( this->drawMenuItem( buf, 1) )
        {
            this->goForward( OSDMenuId::Contrast, config.camera.contrast + 2 );
        }
    }

    {
        char buf[256];
        sprintf(buf, "Exposure: %d##2", config.camera.ae_level);
        if ( this->drawMenuItem( buf, 2) )
        {
            this->goForward( OSDMenuId::Exposure, config.camera.ae_level + 2);
        }
    }

    {
        char buf[256];
        sprintf(buf, "Saturation: %d##3", config.camera.saturation);
        if ( this->drawMenuItem( buf, 3) )
        {
            this->goForward( OSDMenuId::Saturation, config.camera.saturation + 2 );
        }
    }

    {
        char buf[256];
        const char* sharpnessLevels[] = {"Blur more", "Blur", "Normal", "Sharpen", "Sharpen more"};
        sprintf(buf, "Sharpness: %s##4", sharpnessLevels[clamp((int)config.camera.sharpness,-2,2)+2]);
        if ( this->drawMenuItem( buf, 4) )
        {
            this->goForward( OSDMenuId::Sharpness, config.camera.sharpness + 2 );
        }
    }

    if (!s_isOV5640)  //vertical flip drops framerate by half, useless
    {
        if ( this->drawMenuItem( config.camera.vflip ? "Vertical Flip: Enabled##5" : "Vertical Flip: Disabled##5", 5) )
        {
            config.camera.vflip = !config.camera.vflip;
        }
    }

/*
    {
        if ( this->drawMenuItem( config.camera.hmirror ? "Horizontal Mirror: Enabled##5" : "Horizontal Mirror: Disabled##6", 5) )
        {
            config.camera.hmirror = !config.camera.hmirror;
        }
    }
*/

    if ( this->exitKeyPressed())
    {
        this->goBack();
    }

}

//=======================================================
//=======================================================
void OSDMenu::drawResolutionMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> Resolution" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "640x360 30fps (16:9)", 0) )
    {
        config.camera.resolution = Resolution::VGA16;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "640x480 30fps (4:3)", 1) )
    {
        config.camera.resolution = Resolution::VGA;
        saveAndExit = true;
    }
    

    if ( this->drawMenuItem( "800x456 30fps (16:9)", 2) )
    {
        config.camera.resolution = Resolution::SVGA16;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "800x600 30fps (4:3)", 3) )
    {
        config.camera.resolution = Resolution::SVGA;
        saveAndExit = true;
    }

    if (this->drawMenuItem( s_isOV5640 ? "1024x576 30fps (16:9)" : "1024x576 13fps (16:9)", 4) )
    {
        config.camera.resolution = Resolution::XGA16;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( s_isOV5640 ? "1280x720 30fps (16:9)" : "1280x720 13fps (16:9)", 5) )
    {
        config.camera.resolution = Resolution::HD;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
bool OSDMenu::exitKeyPressed()
{
    return ImGui::IsKeyPressed(ImGuiKey_LeftArrow) || ImGui::IsKeyPressed(ImGuiKey_R) || ImGui::IsKeyPressed(ImGuiKey_G) 
    || ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_MouseRight);
}

//=======================================================
//=======================================================
void OSDMenu::drawBrightnessMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Camera Settings -> Brightness" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "-2", 0) )
    {
        config.camera.brightness = -2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "-1", 1) )
    {
        config.camera.brightness = -1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "0", 2) )
    {
        config.camera.brightness = 0;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "1", 3) )
    {
        config.camera.brightness = 1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "2", 4) )
    {
        config.camera.brightness = 2;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}


//=======================================================
//=======================================================
void OSDMenu::drawContrastMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Camera Settings -> Contrast" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "-2", 0) )
    {
        config.camera.contrast = -2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "-1", 1) )
    {
        config.camera.contrast = -1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "0", 2) )
    {
        config.camera.contrast = 0;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "1", 3) )
    {
        config.camera.contrast = 1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "2", 4) )
    {
        config.camera.contrast = 2;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawExposureMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Camera Settings -> Exposure" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "-2", 0) )
    {
        config.camera.ae_level = -2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "-1", 1) )
    {
        config.camera.ae_level = -1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "0", 2) )
    {
        config.camera.ae_level = 0;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "1", 3) )
    {
        config.camera.ae_level = 1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "2", 4) )
    {
        config.camera.ae_level = 2;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawSaturationMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Camera Settings -> Saturation" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "-2", 0) )
    {
        config.camera.saturation = -2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "-1", 1) )
    {
        config.camera.saturation = -1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "0", 2) )
    {
        config.camera.saturation = 0;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "1", 3) )
    {
        config.camera.saturation = 1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "2", 4) )
    {
        config.camera.saturation = 2;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawSharpnessMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Camera Settings -> Sharpness" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "Blue more", 0) )
    {
        config.camera.sharpness = -2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Blur", 1) )
    {
        config.camera.sharpness = -1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Normal", 2) )
    {
        config.camera.sharpness = 0;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Sharpen", 3) )
    {
        config.camera.sharpness = 1;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Sharpen more", 4) )
    {
        config.camera.sharpness = 2;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawExitToShellMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Exit To Shell ?" );
    ImGui::Spacing();

    if ( this->drawMenuItem( "Exit", 0) )
    {
        exitApp();
    }

    bool b = false;
    if ( this->drawMenuItem( "Cancel", 1) )
    {
        b= true;
    }

    if ( b || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawLetterboxMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "GS Settings -> Letterbox" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "Stretch", 0) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::STRETCH;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Letterbox", 1) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::LETTERBOX;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Letterbox, Screen is 5:4", 2) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::ASPECT5X4;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Letterbox, Screen is 4:3", 3) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::ASPECT4X3;
        saveAndExit = true;
    }


    if ( this->drawMenuItem( "Letterbox, Screen is 16:9", 4) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::ASPECT16X9;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Letterbox, Screen is 16:10", 5) )
    {
        s_groundstation_config.screenAspectRatio = ScreenAspectRatio::ASPECT16X10;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGroundStationConfig();
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawWifiRateMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> Wifi Rate" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "OFDM 18Mbps", 0) )
    {
        config.wifi_rate = WIFI_Rate::RATE_G_18M_ODFM;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "OFDM 24Mbps", 1) )
    {
        config.wifi_rate = WIFI_Rate::RATE_G_24M_ODFM;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "OFDM 36Mbps", 2) )
    {
        config.wifi_rate = WIFI_Rate::RATE_G_36M_ODFM;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "MCS2L 19.5Mbps", 3) )
    {
        config.wifi_rate = WIFI_Rate::RATE_N_19_5M_MCS2;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "MCS3L 26Mbps", 4) )
    {
        config.wifi_rate = WIFI_Rate::RATE_N_26M_MCS3;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "MCS4L 39Mbps", 5) )
    {
        config.wifi_rate = WIFI_Rate::RATE_N_39M_MCS4;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawWifiChannelMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> Wifi Channel" );
    ImGui::Spacing();

    bool saveAndExit = false;
    bool bExit = false;

    for ( int i = 0; i < 13; i++ )
    {
        char buf[12];
        sprintf(buf, "%d", i+1);
        if ( this->drawMenuItem( buf, i, true) )
        {
            if ( s_groundstation_config.wifi_channel != (i+1)) 
            {
                s_groundstation_config.wifi_channel = i+1;
                saveAndExit = true;
            }
            else
            {
                bExit = true;
            }
        }
    }

    if ( saveAndExit )
    {
        saveGroundStationConfig();

        restart_tp = Clock::now();
        bRestart = true;

        this->menuId = OSDMenuId::Restart;
    }

    if ( bExit || this->exitKeyPressed() )
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawFECMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> FEC" );
    ImGui::Spacing();

    bool saveAndExit = false;

    if ( this->drawMenuItem( "Weak (6/8)", 0) )
    {
        config.fec_codec_n = 8;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Medium (6/10)", 1) )
    {
        config.fec_codec_n = 10;
        saveAndExit = true;
    }

    if ( this->drawMenuItem( "Strong (6/12)", 2) )
    {
        config.fec_codec_n = 12;
        saveAndExit = true;
    }

    if ( saveAndExit )
    {
        saveGround2AirConfig(config);
    }

    if ( saveAndExit || this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawGSSettingsMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Menu -> GS Settings" );
    ImGui::Spacing();

    {
        char buf[512];
        sprintf(buf, "OSD Font: %s", g_osd.currentFontName);
        if (strlen(buf) > 30 )
        {
            buf[28]='.'; buf[29]='.'; buf[30]='.'; buf[31]=0;
        }
        strcat(buf, "##1");
        if ( this->drawMenuItem( buf, 0) )
        {
            auto it = std::find(g_osd.fontsList.begin(), g_osd.fontsList.end(), g_osd.currentFontName);
            this->goForward( OSDMenuId::OSDFont, it != g_osd.fontsList.end() ? std::distance(g_osd.fontsList.begin(), it) : 0 );
        }
    }

    {
        char buf[256];
        const char* modes[] = {"Stretch", "Letterbox", "Screen is 5:4", "Screen is 4:3", "Screen is 16:9", "Screen is 16:10"};
        sprintf(buf, "Letterbox: %s##1", modes[clamp((int)s_groundstation_config.screenAspectRatio,0,5)]);
        if ( this->drawMenuItem( buf, 1) )
        {
            this->goForward( OSDMenuId::Letterbox, (int)s_groundstation_config.screenAspectRatio );
        }
    }

    if ( this->drawMenuItem( "Exit To Shell", 2) )
    {
        this->goForward( OSDMenuId::ExitToShell, 0 );
    }

    if ( this->exitKeyPressed())
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::drawRestartMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "Restarting..." );
}

//=======================================================
//=======================================================
void OSDMenu::drawOSDFontMenu(Ground2Air_Config_Packet& config)
{
    this->drawMenuTitle( "GS -> Displayport OSD Font" );
    ImGui::Spacing();

    bool bExit = false;

    for ( unsigned int i = 0; i < g_osd.fontsList.size(); i++ )
    {
        char buf[512];
        sprintf(buf, "%s", g_osd.fontsList[i].c_str());
        if (strlen(buf) > 30 )
        {
            buf[28]='.'; buf[29]='.'; buf[30]='.'; buf[31]=0;
        }
        if ( this->drawMenuItem( buf, i, true) )
        {
            if ( strcmp( g_osd.currentFontName, g_osd.fontsList[i].c_str())!= 0) 
            {
                g_osd.loadFont(g_osd.fontsList[i].c_str());
                ini["gs"]["osd_font"] = g_osd.fontsList[i];
                s_iniFile.write(ini);
                bExit = true;
            }
            else
            {
                bExit = true;
            }
        }
    }

    if ( bExit || this->exitKeyPressed() )
    {
        this->goBack();
    }
}

//=======================================================
//=======================================================
void OSDMenu::goForward(OSDMenuId newMenuId, int newItem)
{
    this->backMenuIds.push_back(this->menuId);
    this->backMenuItems.push_back(this->selectedItem);

    this->menuId = newMenuId;
    this->selectedItem = newItem;
}
//=======================================================
//=======================================================
void OSDMenu::goBack()
{
    if ( this->backMenuIds.size() > 0 )
    {
        this->menuId = this->backMenuIds.back();
        this->backMenuIds.pop_back();
        this->selectedItem = this->backMenuItems.back();
        this->backMenuItems.pop_back();
    }
}



