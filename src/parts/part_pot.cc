/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2017  Luis Claudio Gambôa Lopes

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

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"part_pot.h"

/* outputs */
enum
{
  O_P1, O_P2, O_P3, O_P4, O_PO1, O_PO2, O_PO3, O_PO4
};

/* inputs */
enum
{
  I_PO1, I_PO2, I_PO3, I_PO4
};

cpart_pot::cpart_pot (unsigned x, unsigned y)
{
  X = x;
  Y = y;
  ReadMaps ();

  lxImage image;
  image.LoadFile (Window1.GetSharePath () +lxT("parts/")+ GetPictureFileName ());

  Bitmap = new lxBitmap (image);

  canvas.Create (Bitmap);


  input_pins[0] = 0;
  input_pins[1] = 0;
  input_pins[2] = 0;
  input_pins[3] = 0;
  
  values[0] = 0;
  values[1] = 0;
  values[2] = 0;
  values[3] = 0;
  
  active[0] = 0;
  active[1] = 0;
  active[2] = 0;
  active[3] = 0;

};

cpart_pot::~cpart_pot (void)
{
  delete Bitmap;
}

void
cpart_pot::Draw (void)
{



  int i;
  board *pboard = Window1.GetBoard ();

  canvas.Init ();
  
  lxFont font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD );
  canvas.SetFont (font);
  
  for (i = 0; i < outputc; i++)
    {

      switch (output[i].id)
        {
        case O_P1:
        case O_P2:
        case O_P3:
        case O_P4:
          canvas.SetColor (49, 61, 99);
          canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
          canvas.SetFgColor (255, 255, 255);
          if (input_pins[output[i].id-O_P1] == 0)
           canvas.Text ("NC", output[i].x1, output[i].y1);
          else
            canvas.Text (pboard->MGetPinName (input_pins[output[i].id-O_P1]), output[i].x1, output[i].y1);         
          break;
        case O_PO1:         
        case O_PO2:        
        case O_PO3:       
        case O_PO4:  
          canvas.SetColor (50, 50, 50);
          canvas.Rectangle (1,output[i].x1,output[i].y1,output[i].x2-output[i].x1,output[i].y2-output[i].y1);
          canvas.SetColor (250, 250, 250);
          canvas.Rectangle (1,output[i].x1+6,output[i].y1+values[output[i].id-O_PO1],20,10);
          break;          
        break;
        }


    };

  canvas.End ();

}


void
cpart_pot::Process (void)
{


  if (refresh > 100000)
    {
      refresh = 0;

      board *pboard = Window1.GetBoard ();

      pboard->MSetAPin (input_pins[0], 5.0*(148-values[0])/148.0);
      pboard->MSetAPin (input_pins[1], 5.0*(148-values[1])/148.0);
      pboard->MSetAPin (input_pins[2], 5.0*(148-values[2])/148.0);
      pboard->MSetAPin (input_pins[3], 5.0*(148-values[3])/148.0);

    }
  refresh++;

}

void
cpart_pot::MouseButtonPress (uint button, uint x, uint y, uint state)
{

  int i;
  unsigned int l;
  
  for (i = 0; i < inputc; i++)
    {
      if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
        {
          l=(input[i].y2-input[i].y1-10); 
          switch (input[i].id)
            {
            case I_PO1:
              values[0]=y-input[i].y1;
              if( values[0] >= l)values[0]=l;
              active[0]=1;
              break;
            case I_PO2:
              values[1]=y-input[i].y1;
              if( values[1] >= l)values[1]=l;
              active[1]=1;
              break;
            case I_PO3:
              values[2]=y-input[i].y1;
              if( values[2] >= l)values[2]=l;
              active[2]=1;
              break;
            case I_PO4:
              values[3]=y-input[i].y1;
              if( values[3] >= l)values[3]=l;
              active[3]=1;
              break;  
            }    
        }
    }

}

void 
cpart_pot::MouseButtonRelease(uint button, uint x, uint y,uint state)
{
  int i;
  
  for (i = 0; i < inputc; i++)
    {
      if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
        {
          switch (input[i].id)
            {
            case I_PO1:
              active[0]=0;
              break;
            case I_PO2:
              active[1]=0;
              break;
            case I_PO3:
              active[2]=0;
              break;
            case I_PO4:
              active[3]=0;
              break;  
            }    
        }
    }
}

void 
cpart_pot::MouseMove(uint button, uint x, uint y,uint state)
{
  
  int i;
  unsigned int l;
  
  for (i = 0; i < inputc; i++)
    {
      if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
        {
          l=(input[i].y2-input[i].y1-10); 
          switch (input[i].id)
            {
            case I_PO1:
              if(active[0])
              {
                values[0]=y-input[i].y1;
                if( values[0] >= l)values[0]=l;
              }
              break;
            case I_PO2:
              if(active[1])
              {
                values[1]=y-input[i].y1;
                if( values[1] >= l)values[1]=l;
              }
              break;
            case I_PO3:
              if(active[2])
              {
                values[2]=y-input[i].y1;
                if( values[2] >= l)values[2]=l;
              }
              break;
            case I_PO4:
              if(active[3])
              {
                values[3]=y-input[i].y1;
                if( values[3] >= l)values[3]=l;
              }
              break;  
            }    
        }
    }
}

unsigned short
cpart_pot::get_in_id (char * name)
{

  if (strcmp (name, "PO1") == 0)return I_PO1;
  if (strcmp (name, "PO2") == 0)return I_PO2;
  if (strcmp (name, "PO3") == 0)return I_PO3;
  if (strcmp (name, "PO4") == 0)return I_PO4;

  printf ("Erro input '%s' don't have a valid id! \n", name);
  return -1;
};

unsigned short
cpart_pot::get_out_id (char * name)
{

  if (strcmp (name, "P1") == 0)return O_P1;
  if (strcmp (name, "P2") == 0)return O_P2;
  if (strcmp (name, "P3") == 0)return O_P3;
  if (strcmp (name, "P4") == 0)return O_P4;

  if (strcmp (name, "PO1") == 0)return O_PO1;
  if (strcmp (name, "PO2") == 0)return O_PO2;
  if (strcmp (name, "PO3") == 0)return O_PO3;
  if (strcmp (name, "PO4") == 0)return O_PO4;

  printf ("Erro output '%s' don't have a valid id! \n", name);
  return 1;
};

String
cpart_pot::WritePreferences (void)
{
  char prefs[256];

  sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], values[0], values[1], values[2], values[3]);

  return prefs;
};

void
cpart_pot::ReadPreferences (String value)
{
  sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &values[0], &values[1], &values[2], &values[3]);
};

CPWindow * WProp_pot;

void
cpart_pot::ConfigurePropertiesWindow (CPWindow * wprop)
{
  String Items = "0  NC,";
  String spin;
  WProp_pot = wprop;
  board *pboard = Window1.GetBoard ();

  for (int i = 1; i <= pboard->MGetPinCount (); i++)
    {
      spin = pboard->MGetPinName (i);

      if (spin.Cmp (lxT ("error")))
        {
          Items = Items + itoa (i) + "  " + spin + ",";
        }
    }

  ((CCombo*) WProp_pot->GetChildByName ("combo1"))->SetItems (Items);
  if (input_pins[0] == 0)
    ((CCombo*) WProp_pot->GetChildByName ("combo1"))->SetText ("0  NC");
  else
    {
      spin = pboard->MGetPinName (input_pins[0]);
      ((CCombo*) WProp_pot->GetChildByName ("combo1"))->SetText (itoa (input_pins[0]) + "  " + spin);
    }

  ((CCombo*) WProp_pot->GetChildByName ("combo2"))->SetItems (Items);
  if (input_pins[1] == 0)
    ((CCombo*) WProp_pot->GetChildByName ("combo2"))->SetText ("0  NC");
  else
    {
      spin = pboard->MGetPinName (input_pins[1]);
      ((CCombo*) WProp_pot->GetChildByName ("combo2"))->SetText (itoa (input_pins[1]) + "  " + spin);
    }

  ((CCombo*) WProp_pot->GetChildByName ("combo3"))->SetItems (Items);
  if (input_pins[3] == 0)
    ((CCombo*) WProp_pot->GetChildByName ("combo3"))->SetText ("0  NC");
  else
    {
      spin = pboard->MGetPinName (input_pins[2]);
      ((CCombo*) WProp_pot->GetChildByName ("combo3"))->SetText (itoa (input_pins[2]) + "  " + spin);
    }

  ((CCombo*) WProp_pot->GetChildByName ("combo4"))->SetItems (Items);
  if (input_pins[3] == 0)
    ((CCombo*) WProp_pot->GetChildByName ("combo4"))->SetText ("0  NC");
  else
    {
      spin = pboard->MGetPinName (input_pins[3]);
      ((CCombo*) WProp_pot->GetChildByName ("combo4"))->SetText (itoa (input_pins[3]) + "  " + spin);
    }


  ((CButton*) WProp_pot->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pot::PropButton;
  ((CButton*) WProp_pot->GetChildByName ("button1"))->SetTag (1);

  ((CButton*) WProp_pot->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & cpart_pot::PropButton;
}

void
cpart_pot::ReadPropertiesWindow (void)
{
  if (WProp_pot->GetTag ())
    {
      input_pins[0] = atoi (((CCombo*) WProp_pot->GetChildByName ("combo1"))->GetText ());
      input_pins[1] = atoi (((CCombo*) WProp_pot->GetChildByName ("combo2"))->GetText ());
      input_pins[2] = atoi (((CCombo*) WProp_pot->GetChildByName ("combo3"))->GetText ());
      input_pins[3] = atoi (((CCombo*) WProp_pot->GetChildByName ("combo4"))->GetText ());
    }
}

void
cpart_pot::PropButton (CControl * control, uint button, uint x, uint y, uint state)
{
  WProp_pot->SetTag (control->GetTag ());
  WProp_pot->HideExclusive ();
};
