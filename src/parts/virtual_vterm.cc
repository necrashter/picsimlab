/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include "virtual_vterm.h"
#include "../lib/oscilloscope.h"
#include "../lib/picsimlab.h"
#include "../lib/spareparts.h"

/* outputs */
enum { O_RX, O_TX, O_LTX, O_LRX, O_VT };

/* inputs */
enum { I_TERM, I_VT };

/* line ending*/
enum { LE_NONE, LE_NL, LE_CR, LE_NL_CR, O_TERM };

static PCWProp pcwprop[6] = {{PCW_LABEL, "P1 - VCC,+5V"}, {PCW_COMBO, "P2 - RX"}, {PCW_COMBO, "P3 - TX"},
                             {PCW_LABEL, "P4 - GND,GND"}, {PCW_COMBO, "Speed"},   {PCW_END, ""}};

cpart_vterm::cpart_vterm(const unsigned x, const unsigned y, const char* name, const char* type)
    : part(x, y, name, type), font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD) {
    vterm_init(&vt, PICSimLab.GetBoard());
    vterm_rst(&vt);

    pins[0] = 0;
    pins[1] = 0;

    show = 0;

    send_text = 0;

    vterm_speed = 9600;

    lending = LE_NL;

    vttext = NULL;
    vtedit = NULL;
    vtbtn_send = NULL;
    vtbtn_clear = NULL;
    vtcmb_ending = NULL;
    vtcmb_speed = NULL;

    if (PICSimLab.GetWindow()) {
        wvterm = new CPWindow();
        wvterm->SetName("window1");  // must be the same as in xml
        wvterm->SetVisible(0);
        Application->ACreateWindow(wvterm);
        lxString fname =
            lxGetLocalFile(PICSimLab.GetSharePath() + lxT("parts/Virtual/IO Virtual Term/terminal_1.lxrad"));
        if (wvterm->LoadXMLContextAndCreateChilds(fname)) {
            wvterm->SetVisible(0);
            wvterm->Hide();
            wvterm->SetCanDestroy(false);

            wvterm->EvOnShow = SpareParts.PartEvent;

            // wvterm.Draw ();
            // wvterm.Show ();

            vttext = (CText*)wvterm->GetChildByName("text1");
            vtedit = (CEdit*)wvterm->GetChildByName("edit1");

            vtbtn_send = ((CButton*)wvterm->GetChildByName("button1"));
            vtbtn_clear = ((CButton*)wvterm->GetChildByName("button2"));
            vtcmb_ending = ((CCombo*)wvterm->GetChildByName("combo1"));
            vtcmb_speed = ((CCombo*)wvterm->GetChildByName("combo2"));

            vtbtn_send->EvMouseButtonRelease = SpareParts.PartButtonEvent;
            vtbtn_clear->EvMouseButtonRelease = SpareParts.PartButtonEvent;
            vtcmb_speed->EvOnComboChange = SpareParts.PartEvent;
            vtcmb_ending->EvOnComboChange = SpareParts.PartEvent;
            vtedit->EvKeyboardPress = SpareParts.PartKeyEvent;
        } else {
            printf("PICSimLab: Vterm error loading file %s\n", (const char*)fname.c_str());
            PICSimLab.RegisterError("Vterm error loading file:\n" + fname);
            wvterm->SetVisible(0);
            wvterm->Hide();
            wvterm->SetCanDestroy(false);
        }
    } else {
        wvterm = NULL;
    }

    SetPCWProperties(pcwprop);

    PinCount = 2;
    Pins = pins;
}

cpart_vterm::~cpart_vterm(void) {
    delete Bitmap;
    canvas.Destroy();
    vterm_end(&vt);
    if (wvterm) {
        wvterm->Hide();
        wvterm->DestroyChilds();
        wvterm->SetCanDestroy(true);
        wvterm->WDestroy();
    }
}

void cpart_vterm::RegisterRemoteControl(void) {
    input_ids[I_TERM]->status = &show;
    input_ids[I_VT]->status = &vt;
    input_ids[I_VT]->update = &output_ids[O_VT]->update;
    output_ids[O_VT]->status = &vt;
}

void cpart_vterm::SetId(int _id) {
    part::SetId(_id);
    if (vtbtn_send) {
        vtbtn_send->SetTag(id);
        vtbtn_clear->SetTag(id);
        vtcmb_speed->SetTag(id);
        vtcmb_ending->SetTag(id);
        vtedit->SetTag(id);
        wvterm->SetTag(id);
    }
}

void cpart_vterm::ButtonEvent(CControl* control, uint button, uint x, uint y, uint state) {
    if (control == vtbtn_send) {
        text_to_send = vtedit->GetText();
        vtedit->SetText("");
        send_text = 1;
    } else if (control == vtbtn_clear) {
        vttext->Clear();
    }
}

void cpart_vterm::KeyEvent(CControl* control, uint keysym, uint ukeysym, uint state) {
    if (control == vtedit) {
        if (ukeysym == 13) {
            text_to_send = vtedit->GetText();
            vtedit->SetText("");
            send_text = 1;
        }
    }
}

void cpart_vterm::Event(CControl* control) {
    if (control == vtcmb_speed) {
        vterm_speed = atoi(vtcmb_speed->GetText());
        vterm_set_speed(&vt, vterm_speed);
    } else if (control == vtcmb_ending) {
        if (!vtcmb_ending->GetText().Cmp("No line ending")) {
            lending = LE_NONE;
        } else if (!vtcmb_ending->GetText().Cmp("New line")) {
            lending = LE_NL;
        } else if (!vtcmb_ending->GetText().Cmp("Carriage return")) {
            lending = LE_CR;
        } else if (!vtcmb_ending->GetText().Cmp("Both NL and CR")) {
            lending = LE_NL_CR;
        }
    } else if (control == wvterm) {
        vtedit->SetWidth(wvterm->GetWidth() - 100);
        vttext->SetWidth(wvterm->GetWidth() - 29);

        vtbtn_send->SetX(wvterm->GetWidth() - 87);
        vtbtn_clear->SetX(wvterm->GetWidth() - 90);
        vtcmb_ending->SetX(wvterm->GetWidth() - 405);
        vtcmb_speed->SetX(wvterm->GetWidth() - 215);

        vttext->SetHeight(wvterm->GetHeight() - 119);

        vtbtn_clear->SetY(wvterm->GetHeight() - 74);
        vtcmb_ending->SetY(wvterm->GetHeight() - 73);
        vtcmb_speed->SetY(wvterm->GetHeight() - 73);
    }
}

void cpart_vterm::Reset(void) {
    vterm_rst(&vt);
    vterm_set_speed(&vt, vterm_speed);

    if (!vtcmb_speed)
        return;
    vtcmb_speed->SetText(itoa(vterm_speed));

    switch (lending) {
        case LE_NONE:
            vtcmb_ending->SetText("No line ending");
            break;
        case LE_NL:
            vtcmb_ending->SetText("New line");
            break;
        case LE_CR:
            vtcmb_ending->SetText("Carriage return");
            break;
        case LE_NL_CR:
            vtcmb_ending->SetText("Both NL and CR");
            break;
    }
}

void cpart_vterm::DrawOutput(const unsigned int i) {
    switch (output[i].id) {
        case O_LTX:
            canvas.SetColor(0, (vt.bb_uart.leds & 0x02) * 125, 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            vt.bb_uart.leds &= ~0x02;
            break;
        case O_LRX:
            canvas.SetColor(0, (vt.bb_uart.leds & 0x01) * 250, 0);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
            vt.bb_uart.leds &= ~0x01;
            break;
        case O_TERM:
            char str[SBUFFMAX];

            if (show & 0x80) {
                if (show & 0x01) {
                    wvterm->Show();
                } else {
                    wvterm->Hide();
                }
                show &= ~0x80;
            }

            vt.inMutex->Lock();
            strncpy(str, (char*)vt.buff_in, vt.count_in);
            str[vt.count_in] = 0;
            vt.count_in = 0;
            // printf("Data recv: \n[\n%s]\n", str);
            vt.inMutex->Unlock();
            vttext->Append(str);

            while (vttext->GetCountLines() > 1000) {
                vttext->SetCursorPos(0);
                vttext->DelLine();
            }
            break;
        default:
            canvas.SetColor(49, 61, 99);
            canvas.Rectangle(1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

            canvas.SetFgColor(155, 155, 155);

            int pinv = output[i].id - O_RX;
            switch (pinv) {
                case 0:
                    if (pins[pinv] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(pins[pinv]), output[i].x1, output[i].y2, 90.0);
                case 1:
                    if (pins[pinv] == 0)
                        canvas.RotatedText("NC", output[i].x1, output[i].y2, 90.0);
                    else
                        canvas.RotatedText(SpareParts.GetPinName(pins[pinv]), output[i].x1, output[i].y2, 90.0);
                    break;
            }
            break;
    }
}

unsigned short cpart_vterm::GetInputId(char* name) {
    if (strcmp(name, "PB_TERM") == 0)
        return I_TERM;
    if (strcmp(name, "VT_VTERM") == 0)
        return I_VT;

    printf("Erro input '%s' don't have a valid id! \n", name);
    return -1;
}

unsigned short cpart_vterm::GetOutputId(char* name) {
    if (strcmp(name, "PN_RX") == 0)
        return O_RX;
    if (strcmp(name, "PN_TX") == 0)
        return O_TX;
    if (strcmp(name, "LD_TX") == 0)
        return O_LTX;
    if (strcmp(name, "LD_RX") == 0)
        return O_LRX;
    if (strcmp(name, "PB_TERM") == 0)
        return O_TERM;
    if (strcmp(name, "VT_VTERM") == 0)
        return O_VT;

    printf("Erro output '%s' don't have a valid id! \n", name);
    return 1;
}

lxString cpart_vterm::WritePreferences(void) {
    char prefs[256];

    if (wvterm) {
        sprintf(prefs, "%hhu,%hhu,%hhu,%u,%hhu,%i,%i", pins[0], pins[1], lending, vterm_speed, show, wvterm->GetX(),
                wvterm->GetY());
    } else {
        sprintf(prefs, "%hhu,%hhu,%hhu,%u,%hhu,%i,%i", pins[0], pins[1], lending, vterm_speed, show, 100, 100);
    }

    return prefs;
}

void cpart_vterm::ReadPreferences(lxString value) {
    int x, y;
    sscanf(value.c_str(), "%hhu,%hhu,%hhu,%u,%hhu,%i,%i", &pins[0], &pins[1], &lending, &vterm_speed, &show, &x, &y);
    show |= 0x80;
    if (wvterm) {
        wvterm->SetX(x);
        wvterm->SetY(y);
    }
    Reset();
}

void cpart_vterm::ConfigurePropertiesWindow(CPWindow* WProp) {
    SetPCWComboWithPinNames(WProp, "combo2", pins[0]);
    SetPCWComboWithPinNames(WProp, "combo3", pins[1]);

    ((CCombo*)WProp->GetChildByName("combo5"))->SetItems("1200,2400,4800,9600,19200,38400,57600,115200,");
    ((CCombo*)WProp->GetChildByName("combo5"))->SetText(itoa(vterm_speed));
}

void cpart_vterm::ReadPropertiesWindow(CPWindow* WProp) {
    pins[0] = GetPWCComboSelectedPin(WProp, "combo2");
    pins[1] = GetPWCComboSelectedPin(WProp, "combo3");
    vterm_speed = atoi(((CCombo*)WProp->GetChildByName("combo5"))->GetText());
    Reset();
}

void cpart_vterm::PreProcess(void) {
    if (send_text) {
        if (!vt.count_out) {
            send_text = 0;
            vt.count_out = text_to_send.size();
            for (unsigned int i = 0; i < vt.count_out; i++) {
                vt.buff_out[i] = text_to_send[i];
            }
            switch (lending) {
                case LE_NL:
                    vt.buff_out[vt.count_out++] = '\n';
                    break;
                case LE_CR:
                    vt.buff_out[vt.count_out++] = '\r';
                    break;
                case LE_NL_CR:
                    vt.buff_out[vt.count_out++] = '\r';
                    vt.buff_out[vt.count_out++] = '\n';
                    break;
            }
        }
    }
    Process();  // check for input updates
}

void cpart_vterm::Process(void) {
    int ret = 0;

    unsigned char val = 1;

    const picpin* ppins = SpareParts.GetPinsValues();

    if (pins[0]) {
        val = ppins[pins[0] - 1].value;
    }

    ret = vterm_io(&vt, val);
    SpareParts.SetPin(pins[1], ret);
}

void cpart_vterm::OnMouseButtonPress(uint inputId, uint button, uint x, uint y, uint state) {
    switch (inputId) {
        case I_TERM:
            if (button == 1) {
                show = (show & 0x01) ^ 1;
                show |= 0x80;
            }
            break;
    }
}

void cpart_vterm::PostProcess(void) {
    if (show & 0x80) {
        output_ids[O_TERM]->update = 1;
    }

    if (output_ids[O_LTX]->value != (vt.bb_uart.leds & 0x02)) {
        output_ids[O_LTX]->value = (vt.bb_uart.leds & 0x02);
        output_ids[O_LTX]->update = 1;
    }

    if (output_ids[O_LRX]->value != (vt.bb_uart.leds & 0x01)) {
        output_ids[O_LRX]->value = (vt.bb_uart.leds & 0x01);
        output_ids[O_LRX]->update = 1;
    }
    if (vt.count_in) {
        output_ids[O_TERM]->update = 1;
    }
}

part_init(PART_vterm_Name, cpart_vterm, "Virtual");
