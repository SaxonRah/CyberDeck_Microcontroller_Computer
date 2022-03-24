///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////SD File Managment
#include <SD.h>
#include <SPI.h>

File myFile;
const int chipSelect = BUILTIN_SDCARD;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////SD File Managment

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////USB Host BT Keyboard/Mouse
#include "USBHost_t36.h"

#define HWSERIAL Serial1
#define Serial1_RX 0
#define Serial1_TX 1

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
KeyboardController keyboard1(myusb);
//KeyboardController keyboard2(myusb);
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);
USBHIDParser hid4(myusb);
USBHIDParser hid5(myusb);
MouseController mouse1(myusb);
//JoystickController joystick1(myusb);
//BluetoothController bluet(myusb, true, "0000");   // Version does pairing to device

BluetoothController bluet(myusb);   // version assumes it already was paired

USBDriver *drivers[] = {&hub1, &hub2, &keyboard1, &bluet, &hid1, &hid2, &hid3, &hid4, &hid5};

#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "Hub2", "KB1", "Bluet", "HID1" , "HID2", "HID3", "HID4", "HID5"};

bool driver_active[CNT_DEVICES] = {false, false, false, false};

// Lets also look at HID Input devices
USBHIDInput *hiddrivers[] = {&mouse1};

#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_HIDDEVICES] = {"Mouse1"};

bool hid_driver_active[CNT_HIDDEVICES] = {false};

BTHIDInput *bthiddrivers[] = {&keyboard1, &mouse1};
#define CNT_BTHIDDEVICES (sizeof(bthiddrivers)/sizeof(bthiddrivers[0]))
const char * bthid_driver_names[CNT_BTHIDDEVICES] = {"Keyboard(BT)", "Mouse(BT)"};
bool bthid_driver_active[CNT_BTHIDDEVICES] = {false};

bool show_changed_only = false;

uint8_t joystick_left_trigger_value = 0;
uint8_t joystick_right_trigger_value = 0;
uint64_t joystick_full_notify_mask = (uint64_t) - 1;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////USB Host BT Keyboard/Mouse

void OnPress(int key)
{
  HWSERIAL.print("key '");
  switch (key) {
    case KEYD_UP       : HWSERIAL.print("UP"); break;
    case KEYD_DOWN     : HWSERIAL.print("DN"); break;
    case KEYD_LEFT     : HWSERIAL.print("LEFT"); break;
    case KEYD_RIGHT    : HWSERIAL.print("RIGHT"); break;
    case KEYD_INSERT   : HWSERIAL.print("Ins"); break;
    case KEYD_DELETE   : HWSERIAL.print("Del"); break;
    case KEYD_PAGE_UP  : HWSERIAL.print("PUP"); break;
    case KEYD_PAGE_DOWN: HWSERIAL.print("PDN"); break;
    case KEYD_HOME     : HWSERIAL.print("HOME"); break;
    case KEYD_END      : HWSERIAL.print("<FillScreen,1,0xFFFF,0.0>"); break;
    case KEYD_F1       : HWSERIAL.print("<Function1,10,0xFFFF,0.0>"); Serial.print("F1"); break;
    case KEYD_F2       : HWSERIAL.print("<Function1,20,0x00FF,0.0>"); Serial.print("F2"); break;
    case KEYD_F3       : HWSERIAL.print("<Function1,30,0xFF00,0.0>"); break;
    case KEYD_F4       : HWSERIAL.print("F4"); break;
    case KEYD_F5       : HWSERIAL.print("F5"); break;
    case KEYD_F6       : HWSERIAL.print("F6"); break;
    case KEYD_F7       : HWSERIAL.print("F7"); break;
    case KEYD_F8       : HWSERIAL.print("F8"); break;
    case KEYD_F9       : HWSERIAL.print("F9"); break;
    case KEYD_F10      : HWSERIAL.print("F10"); break;
    case KEYD_F11      : HWSERIAL.print("F11"); break;
    case KEYD_F12      : HWSERIAL.print("F12"); break;
    default: HWSERIAL.print((char)key); break;
  }
  HWSERIAL.print("'  ");
  HWSERIAL.print(key);
  HWSERIAL.print(" MOD: ");
  if (keyboard1) {
    HWSERIAL.print(keyboard1.getModifiers(), HEX);
    HWSERIAL.print(" OEM: ");
    HWSERIAL.print(keyboard1.getOemKey(), HEX);
    HWSERIAL.print(" LEDS: ");
    HWSERIAL.println(keyboard1.LEDS(), HEX);
    //} else {
    //HWSERIAL.print(keyboard2.getModifiers(), HEX);
    //HWSERIAL.print(" OEM: ");
    //HWSERIAL.print(keyboard2.getOemKey(), HEX);
    //HWSERIAL.print(" LEDS: ");
    //HWSERIAL.println(keyboard2.LEDS(), HEX);
  }

  //HWSERIAL.print("key ");
  //HWSERIAL.print((char)keyboard1.getKey());
  //HWSERIAL.print("  ");
  //HWSERIAL.print((char)keyboard2.getKey());
  //HWSERIAL.println();
}
void OnHIDExtrasPress(uint32_t top, uint16_t key)
{
  HWSERIAL.print("HID (");
  HWSERIAL.print(top, HEX);
  HWSERIAL.print(") key press:");
  HWSERIAL.print(key, HEX);
  if (top == 0xc0000) {
    switch (key) {
      case  0x20 : HWSERIAL.print(" - +10"); break;
      case  0x21 : HWSERIAL.print(" - +100"); break;
      case  0x22 : HWSERIAL.print(" - AM/PM"); break;
      case  0x30 : HWSERIAL.print(" - Power"); break;
      case  0x31 : HWSERIAL.print(" - Reset"); break;
      case  0x32 : HWSERIAL.print(" - Sleep"); break;
      case  0x33 : HWSERIAL.print(" - Sleep After"); break;
      case  0x34 : HWSERIAL.print(" - Sleep Mode"); break;
      case  0x35 : HWSERIAL.print(" - Illumination"); break;
      case  0x36 : HWSERIAL.print(" - Function Buttons"); break;
      case  0x40 : HWSERIAL.print(" - Menu"); break;
      case  0x41 : HWSERIAL.print(" - Menu  Pick"); break;
      case  0x42 : HWSERIAL.print(" - Menu Up"); break;
      case  0x43 : HWSERIAL.print(" - Menu Down"); break;
      case  0x44 : HWSERIAL.print(" - Menu Left"); break;
      case  0x45 : HWSERIAL.print(" - Menu Right"); break;
      case  0x46 : HWSERIAL.print(" - Menu Escape"); break;
      case  0x47 : HWSERIAL.print(" - Menu Value Increase"); break;
      case  0x48 : HWSERIAL.print(" - Menu Value Decrease"); break;
      case  0x60 : HWSERIAL.print(" - Data On Screen"); break;
      case  0x61 : HWSERIAL.print(" - Closed Caption"); break;
      case  0x62 : HWSERIAL.print(" - Closed Caption Select"); break;
      case  0x63 : HWSERIAL.print(" - VCR/TV"); break;
      case  0x64 : HWSERIAL.print(" - Broadcast Mode"); break;
      case  0x65 : HWSERIAL.print(" - Snapshot"); break;
      case  0x66 : HWSERIAL.print(" - Still"); break;
      case  0x80 : HWSERIAL.print(" - Selection"); break;
      case  0x81 : HWSERIAL.print(" - Assign Selection"); break;
      case  0x82 : HWSERIAL.print(" - Mode Step"); break;
      case  0x83 : HWSERIAL.print(" - Recall Last"); break;
      case  0x84 : HWSERIAL.print(" - Enter Channel"); break;
      case  0x85 : HWSERIAL.print(" - Order Movie"); break;
      case  0x86 : HWSERIAL.print(" - Channel"); break;
      case  0x87 : HWSERIAL.print(" - Media Selection"); break;
      case  0x88 : HWSERIAL.print(" - Media Select Computer"); break;
      case  0x89 : HWSERIAL.print(" - Media Select TV"); break;
      case  0x8A : HWSERIAL.print(" - Media Select WWW"); break;
      case  0x8B : HWSERIAL.print(" - Media Select DVD"); break;
      case  0x8C : HWSERIAL.print(" - Media Select Telephone"); break;
      case  0x8D : HWSERIAL.print(" - Media Select Program Guide"); break;
      case  0x8E : HWSERIAL.print(" - Media Select Video Phone"); break;
      case  0x8F : HWSERIAL.print(" - Media Select Games"); break;
      case  0x90 : HWSERIAL.print(" - Media Select Messages"); break;
      case  0x91 : HWSERIAL.print(" - Media Select CD"); break;
      case  0x92 : HWSERIAL.print(" - Media Select VCR"); break;
      case  0x93 : HWSERIAL.print(" - Media Select Tuner"); break;
      case  0x94 : HWSERIAL.print(" - Quit"); break;
      case  0x95 : HWSERIAL.print(" - Help"); break;
      case  0x96 : HWSERIAL.print(" - Media Select Tape"); break;
      case  0x97 : HWSERIAL.print(" - Media Select Cable"); break;
      case  0x98 : HWSERIAL.print(" - Media Select Satellite"); break;
      case  0x99 : HWSERIAL.print(" - Media Select Security"); break;
      case  0x9A : HWSERIAL.print(" - Media Select Home"); break;
      case  0x9B : HWSERIAL.print(" - Media Select Call"); break;
      case  0x9C : HWSERIAL.print(" - Channel Increment"); break;
      case  0x9D : HWSERIAL.print(" - Channel Decrement"); break;
      case  0x9E : HWSERIAL.print(" - Media Select SAP"); break;
      case  0xA0 : HWSERIAL.print(" - VCR Plus"); break;
      case  0xA1 : HWSERIAL.print(" - Once"); break;
      case  0xA2 : HWSERIAL.print(" - Daily"); break;
      case  0xA3 : HWSERIAL.print(" - Weekly"); break;
      case  0xA4 : HWSERIAL.print(" - Monthly"); break;
      case  0xB0 : HWSERIAL.print(" - Play"); break;
      case  0xB1 : HWSERIAL.print(" - Pause"); break;
      case  0xB2 : HWSERIAL.print(" - Record"); break;
      case  0xB3 : HWSERIAL.print(" - Fast Forward"); break;
      case  0xB4 : HWSERIAL.print(" - Rewind"); break;
      case  0xB5 : HWSERIAL.print(" - Scan Next Track"); break;
      case  0xB6 : HWSERIAL.print(" - Scan Previous Track"); break;
      case  0xB7 : HWSERIAL.print(" - Stop"); break;
      case  0xB8 : HWSERIAL.print(" - Eject"); break;
      case  0xB9 : HWSERIAL.print(" - Random Play"); break;
      case  0xBA : HWSERIAL.print(" - Select DisC"); break;
      case  0xBB : HWSERIAL.print(" - Enter Disc"); break;
      case  0xBC : HWSERIAL.print(" - Repeat"); break;
      case  0xBD : HWSERIAL.print(" - Tracking"); break;
      case  0xBE : HWSERIAL.print(" - Track Normal"); break;
      case  0xBF : HWSERIAL.print(" - Slow Tracking"); break;
      case  0xC0 : HWSERIAL.print(" - Frame Forward"); break;
      case  0xC1 : HWSERIAL.print(" - Frame Back"); break;
      case  0xC2 : HWSERIAL.print(" - Mark"); break;
      case  0xC3 : HWSERIAL.print(" - Clear Mark"); break;
      case  0xC4 : HWSERIAL.print(" - Repeat From Mark"); break;
      case  0xC5 : HWSERIAL.print(" - Return To Mark"); break;
      case  0xC6 : HWSERIAL.print(" - Search Mark Forward"); break;
      case  0xC7 : HWSERIAL.print(" - Search Mark Backwards"); break;
      case  0xC8 : HWSERIAL.print(" - Counter Reset"); break;
      case  0xC9 : HWSERIAL.print(" - Show Counter"); break;
      case  0xCA : HWSERIAL.print(" - Tracking Increment"); break;
      case  0xCB : HWSERIAL.print(" - Tracking Decrement"); break;
      case  0xCD : HWSERIAL.print(" - Pause/Continue"); break;
      case  0xE0 : HWSERIAL.print(" - Volume"); break;
      case  0xE1 : HWSERIAL.print(" - Balance"); break;
      case  0xE2 : HWSERIAL.print(" - Mute"); break;
      case  0xE3 : HWSERIAL.print(" - Bass"); break;
      case  0xE4 : HWSERIAL.print(" - Treble"); break;
      case  0xE5 : HWSERIAL.print(" - Bass Boost"); break;
      case  0xE6 : HWSERIAL.print(" - Surround Mode"); break;
      case  0xE7 : HWSERIAL.print(" - Loudness"); break;
      case  0xE8 : HWSERIAL.print(" - MPX"); break;
      case  0xE9 : HWSERIAL.print(" - Volume Up"); break;
      case  0xEA : HWSERIAL.print(" - Volume Down"); break;
      case  0xF0 : HWSERIAL.print(" - Speed Select"); break;
      case  0xF1 : HWSERIAL.print(" - Playback Speed"); break;
      case  0xF2 : HWSERIAL.print(" - Standard Play"); break;
      case  0xF3 : HWSERIAL.print(" - Long Play"); break;
      case  0xF4 : HWSERIAL.print(" - Extended Play"); break;
      case  0xF5 : HWSERIAL.print(" - Slow"); break;
      case  0x100: HWSERIAL.print(" - Fan Enable"); break;
      case  0x101: HWSERIAL.print(" - Fan Speed"); break;
      case  0x102: HWSERIAL.print(" - Light"); break;
      case  0x103: HWSERIAL.print(" - Light Illumination Level"); break;
      case  0x104: HWSERIAL.print(" - Climate Control Enable"); break;
      case  0x105: HWSERIAL.print(" - Room Temperature"); break;
      case  0x106: HWSERIAL.print(" - Security Enable"); break;
      case  0x107: HWSERIAL.print(" - Fire Alarm"); break;
      case  0x108: HWSERIAL.print(" - Police Alarm"); break;
      case  0x150: HWSERIAL.print(" - Balance Right"); break;
      case  0x151: HWSERIAL.print(" - Balance Left"); break;
      case  0x152: HWSERIAL.print(" - Bass Increment"); break;
      case  0x153: HWSERIAL.print(" - Bass Decrement"); break;
      case  0x154: HWSERIAL.print(" - Treble Increment"); break;
      case  0x155: HWSERIAL.print(" - Treble Decrement"); break;
      case  0x160: HWSERIAL.print(" - Speaker System"); break;
      case  0x161: HWSERIAL.print(" - Channel Left"); break;
      case  0x162: HWSERIAL.print(" - Channel Right"); break;
      case  0x163: HWSERIAL.print(" - Channel Center"); break;
      case  0x164: HWSERIAL.print(" - Channel Front"); break;
      case  0x165: HWSERIAL.print(" - Channel Center Front"); break;
      case  0x166: HWSERIAL.print(" - Channel Side"); break;
      case  0x167: HWSERIAL.print(" - Channel Surround"); break;
      case  0x168: HWSERIAL.print(" - Channel Low Frequency Enhancement"); break;
      case  0x169: HWSERIAL.print(" - Channel Top"); break;
      case  0x16A: HWSERIAL.print(" - Channel Unknown"); break;
      case  0x170: HWSERIAL.print(" - Sub-channel"); break;
      case  0x171: HWSERIAL.print(" - Sub-channel Increment"); break;
      case  0x172: HWSERIAL.print(" - Sub-channel Decrement"); break;
      case  0x173: HWSERIAL.print(" - Alternate Audio Increment"); break;
      case  0x174: HWSERIAL.print(" - Alternate Audio Decrement"); break;
      case  0x180: HWSERIAL.print(" - Application Launch Buttons"); break;
      case  0x181: HWSERIAL.print(" - AL Launch Button Configuration Tool"); break;
      case  0x182: HWSERIAL.print(" - AL Programmable Button Configuration"); break;
      case  0x183: HWSERIAL.print(" - AL Consumer Control Configuration"); break;
      case  0x184: HWSERIAL.print(" - AL Word Processor"); break;
      case  0x185: HWSERIAL.print(" - AL Text Editor"); break;
      case  0x186: HWSERIAL.print(" - AL Spreadsheet"); break;
      case  0x187: HWSERIAL.print(" - AL Graphics Editor"); break;
      case  0x188: HWSERIAL.print(" - AL Presentation App"); break;
      case  0x189: HWSERIAL.print(" - AL Database App"); break;
      case  0x18A: HWSERIAL.print(" - AL Email Reader"); break;
      case  0x18B: HWSERIAL.print(" - AL Newsreader"); break;
      case  0x18C: HWSERIAL.print(" - AL Voicemail"); break;
      case  0x18D: HWSERIAL.print(" - AL Contacts/Address Book"); break;
      case  0x18E: HWSERIAL.print(" - AL Calendar/Schedule"); break;
      case  0x18F: HWSERIAL.print(" - AL Task/Project Manager"); break;
      case  0x190: HWSERIAL.print(" - AL Log/Journal/Timecard"); break;
      case  0x191: HWSERIAL.print(" - AL Checkbook/Finance"); break;
      case  0x192: HWSERIAL.print(" - AL Calculator"); break;
      case  0x193: HWSERIAL.print(" - AL A/V Capture/Playback"); break;
      case  0x194: HWSERIAL.print(" - AL Local Machine Browser"); break;
      case  0x195: HWSERIAL.print(" - AL LAN/WAN Browser"); break;
      case  0x196: HWSERIAL.print(" - AL Internet Browser"); break;
      case  0x197: HWSERIAL.print(" - AL Remote Networking/ISP Connect"); break;
      case  0x198: HWSERIAL.print(" - AL Network Conference"); break;
      case  0x199: HWSERIAL.print(" - AL Network Chat"); break;
      case  0x19A: HWSERIAL.print(" - AL Telephony/Dialer"); break;
      case  0x19B: HWSERIAL.print(" - AL Logon"); break;
      case  0x19C: HWSERIAL.print(" - AL Logoff"); break;
      case  0x19D: HWSERIAL.print(" - AL Logon/Logoff"); break;
      case  0x19E: HWSERIAL.print(" - AL Terminal Lock/Screensaver"); break;
      case  0x19F: HWSERIAL.print(" - AL Control Panel"); break;
      case  0x1A0: HWSERIAL.print(" - AL Command Line Processor/Run"); break;
      case  0x1A1: HWSERIAL.print(" - AL Process/Task Manager"); break;
      case  0x1A2: HWSERIAL.print(" - AL Select Tast/Application"); break;
      case  0x1A3: HWSERIAL.print(" - AL Next Task/Application"); break;
      case  0x1A4: HWSERIAL.print(" - AL Previous Task/Application"); break;
      case  0x1A5: HWSERIAL.print(" - AL Preemptive Halt Task/Application"); break;
      case  0x200: HWSERIAL.print(" - Generic GUI Application Controls"); break;
      case  0x201: HWSERIAL.print(" - AC New"); break;
      case  0x202: HWSERIAL.print(" - AC Open"); break;
      case  0x203: HWSERIAL.print(" - AC Close"); break;
      case  0x204: HWSERIAL.print(" - AC Exit"); break;
      case  0x205: HWSERIAL.print(" - AC Maximize"); break;
      case  0x206: HWSERIAL.print(" - AC Minimize"); break;
      case  0x207: HWSERIAL.print(" - AC Save"); break;
      case  0x208: HWSERIAL.print(" - AC Print"); break;
      case  0x209: HWSERIAL.print(" - AC Properties"); break;
      case  0x21A: HWSERIAL.print(" - AC Undo"); break;
      case  0x21B: HWSERIAL.print(" - AC Copy"); break;
      case  0x21C: HWSERIAL.print(" - AC Cut"); break;
      case  0x21D: HWSERIAL.print(" - AC Paste"); break;
      case  0x21E: HWSERIAL.print(" - AC Select All"); break;
      case  0x21F: HWSERIAL.print(" - AC Find"); break;
      case  0x220: HWSERIAL.print(" - AC Find and Replace"); break;
      case  0x221: HWSERIAL.print(" - AC Search"); break;
      case  0x222: HWSERIAL.print(" - AC Go To"); break;
      case  0x223: HWSERIAL.print(" - AC Home"); break;
      case  0x224: HWSERIAL.print(" - AC Back"); break;
      case  0x225: HWSERIAL.print(" - AC Forward"); break;
      case  0x226: HWSERIAL.print(" - AC Stop"); break;
      case  0x227: HWSERIAL.print(" - AC Refresh"); break;
      case  0x228: HWSERIAL.print(" - AC Previous Link"); break;
      case  0x229: HWSERIAL.print(" - AC Next Link"); break;
      case  0x22A: HWSERIAL.print(" - AC Bookmarks"); break;
      case  0x22B: HWSERIAL.print(" - AC History"); break;
      case  0x22C: HWSERIAL.print(" - AC Subscriptions"); break;
      case  0x22D: HWSERIAL.print(" - AC Zoom In"); break;
      case  0x22E: HWSERIAL.print(" - AC Zoom Out"); break;
      case  0x22F: HWSERIAL.print(" - AC Zoom"); break;
      case  0x230: HWSERIAL.print(" - AC Full Screen View"); break;
      case  0x231: HWSERIAL.print(" - AC Normal View"); break;
      case  0x232: HWSERIAL.print(" - AC View Toggle"); break;
      case  0x233: HWSERIAL.print(" - AC Scroll Up"); break;
      case  0x234: HWSERIAL.print(" - AC Scroll Down"); break;
      case  0x235: HWSERIAL.print(" - AC Scroll"); break;
      case  0x236: HWSERIAL.print(" - AC Pan Left"); break;
      case  0x237: HWSERIAL.print(" - AC Pan Right"); break;
      case  0x238: HWSERIAL.print(" - AC Pan"); break;
      case  0x239: HWSERIAL.print(" - AC New Window"); break;
      case  0x23A: HWSERIAL.print(" - AC Tile Horizontally"); break;
      case  0x23B: HWSERIAL.print(" - AC Tile Vertically"); break;
      case  0x23C: HWSERIAL.print(" - AC Format"); break;

    }
  }
  HWSERIAL.println();
}

void OnHIDExtrasRelease(uint32_t top, uint16_t key)
{
  HWSERIAL.print("HID (");
  HWSERIAL.print(top, HEX);
  HWSERIAL.print(") key release:");
  HWSERIAL.println(key, HEX);
}

//=============================================================================
// UpdateActiveDeviceInfo
//=============================================================================
void UpdateActiveDeviceInfo() {
  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        HWSERIAL.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        HWSERIAL.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) HWSERIAL.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) HWSERIAL.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) HWSERIAL.printf("  HWSERIAL: %s\n", psz);

        if (drivers[i] == &bluet) {
          const uint8_t *bdaddr = bluet.myBDAddr();
          // remember it...
          HWSERIAL.printf("  BDADDR: %x:%x:%x:%x:%x:%x\n", bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4], bdaddr[5]);
        }
      }
    }
  }
  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) {
    if (*hiddrivers[i] != hid_driver_active[i]) {
      if (hid_driver_active[i]) {
        HWSERIAL.printf("*** HID Device %s - disconnected ***\n", hid_driver_names[i]);
        hid_driver_active[i] = false;
      } else {
        HWSERIAL.printf("*** HID Device %s %x:%x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) HWSERIAL.printf("  manufacturer: %s\n", psz);
        psz = hiddrivers[i]->product();
        if (psz && *psz) HWSERIAL.printf("  product: %s\n", psz);
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) HWSERIAL.printf("  HWSERIAL: %s\n", psz);
      }
    }
  }
  // Then Bluetooth devices
  for (uint8_t i = 0; i < CNT_BTHIDDEVICES; i++) {
    if (*bthiddrivers[i] != bthid_driver_active[i]) {
      if (bthid_driver_active[i]) {
        HWSERIAL.printf("*** BTHID Device %s - disconnected ***\n", bthid_driver_names[i]);
        bthid_driver_active[i] = false;
      } else {
        HWSERIAL.printf("*** BTHID Device %s - connected ***\n", bthid_driver_names[i]); HWSERIAL.flush();
        bthid_driver_active[i] = true;
        #if 0

        const uint8_t *psz = bthiddrivers[i]->manufacturer();
        if (psz && *psz) HWSERIAL.printf("  manufacturer: %s\n", psz);
        psz = bthiddrivers[i]->product();
        if (psz && *psz) HWSERIAL.printf("  product: %s\n", psz);
        psz = bthiddrivers[i]->serialNumber();
        if (psz && *psz) HWSERIAL.printf("  HWSERIAL: %s\n", psz);
    #endif  
      }
    }
  }
}

void setup_bt_keyboard()
{
  HWSERIAL.setRX(Serial1_RX);
  HWSERIAL.setTX(Serial1_TX);
  
//  HWSERIAL.begin(9600);
//  HWSERIAL.begin(19200);
//  HWSERIAL.begin(28800);
//  HWSERIAL.begin(38400);
//  HWSERIAL.begin(57600);
//  HWSERIAL.begin(76800);
//  HWSERIAL.begin(115200);
//  HWSERIAL.begin(230400);
//  HWSERIAL.begin(460800);
//  HWSERIAL.begin(576000);
  HWSERIAL.begin(921600);

  HWSERIAL.println("\n\nUSB Host Testing");
  HWSERIAL.println(sizeof(USBHub), DEC);
  myusb.begin();
  keyboard1.attachPress(OnPress);
  //keyboard2.attachPress(OnPress);
  keyboard1.attachExtrasPress(OnHIDExtrasPress);
  keyboard1.attachExtrasRelease(OnHIDExtrasRelease);
  //keyboard2.attachExtrasPress(OnHIDExtrasPress);
  //keyboard2.attachExtrasRelease(OnHIDExtrasRelease);
}

void bt_keyboard_loop()
{
  // check to see if the device list has changed:
  UpdateActiveDeviceInfo();

  myusb.Task();

  if (mouse1.available()) {
    HWSERIAL.print("Mouse: buttons = ");
    HWSERIAL.print(mouse1.getButtons());
    HWSERIAL.print(",  mouseX = ");
    HWSERIAL.print(mouse1.getMouseX());
    HWSERIAL.print(",  mouseY = ");
    HWSERIAL.print(mouse1.getMouseY());
    HWSERIAL.print(",  wheel = ");
    HWSERIAL.print(mouse1.getWheel());
    HWSERIAL.print(",  wheelH = ");
    HWSERIAL.print(mouse1.getWheelH());
    HWSERIAL.println();
    mouse1.mouseDataClear();
  }
}

void sd_write_read()
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // open the file. 
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
  // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void setup()
{
  //while (!HWSERIAL) ; // wait for Arduino HWSERIAL Monitor
  Serial.begin(9600);
  
  setup_bt_keyboard();
  sd_write_read();
}


void loop()
{
  bt_keyboard_loop();
}
