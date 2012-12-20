#include "../AGE_Frame.h"
using boost::lexical_cast;

string AGE_Frame::GetPlayerColorName(short Index)
{
	if(GameVersion <= 1)
		return GenieFile->PlayerColours[Index].Name;
	return "Color "+lexical_cast<string>(Index)+" ";
}

void AGE_Frame::ListPlayerColors()
{
	wxString Name;
	searchText = Colors_Colors_Search->GetValue().Lower();
	excludeText = Colors_Colors_Search_R->GetValue().Lower();

	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Colors_Colors_List->GetCount() > 0) Colors_Colors_List->Clear();

	for(short loop=0; loop < GenieFile->PlayerColours.size(); loop++)
	{
		Name = " "+lexical_cast<string>(loop)+" - "+GetPlayerColorName(loop);
		if(SearchMatches(Name.Lower()))
		{
			Colors_Colors_List->Append(Name, (void*)&GenieFile->PlayerColours[loop]);
		}
	}

	ListingFix(Selections, Colors_Colors_List);

	wxCommandEvent E;
	OnPlayerColorsSelect(E);
}

void AGE_Frame::OnPlayerColorsSearch(wxCommandEvent &Event)
{
	ListPlayerColors();
}

void AGE_Frame::OnPlayerColorsSelect(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		ColorIDs.resize(Selections);
		Colors_ID->resize(Selections);
		Colors_ColorL->resize(Selections);
		if(GameVersion < 2)	//	AoE and RoR
		{
			Colors_Name->resize(Selections);
		}
		else	//	Above AoE and RoR
		{
			Colors_Palette->resize(Selections);
			Colors_MinimapColor->resize(Selections);
			Colors_Unknown1->resize(Selections);
			Colors_Unknown2->resize(Selections);
			Colors_Unknown3->resize(Selections);
			Colors_Unknown4->resize(Selections);
			Colors_Unknown5->resize(Selections);
		}

		genie::PlayerColour * PlayerColorPointer;
		for(auto loop = Selections; loop--> 0;)
		{
			PlayerColorPointer = (genie::PlayerColour*)Colors_Colors_List->GetClientData(Items.Item(loop));
			ColorIDs[loop] = (PlayerColorPointer - (&GenieFile->PlayerColours[0]));

			Colors_ID->container[loop] = &PlayerColorPointer->ID;
			Colors_ColorL->container[loop] = &PlayerColorPointer->Colour;
			if(GameVersion < 2)	//	AoE and RoR
			{
				Colors_Name->container[loop] = &PlayerColorPointer->Name;
			}
			else	//	Above AoE and RoR
			{
				Colors_Palette->container[loop] = &PlayerColorPointer->Palette;
				Colors_MinimapColor->container[loop] = &PlayerColorPointer->MinimapColour;
				Colors_Unknown1->container[loop] = &PlayerColorPointer->Unknown1;
				Colors_Unknown2->container[loop] = &PlayerColorPointer->Unknown2;
				Colors_Unknown3->container[loop] = &PlayerColorPointer->Unknown3;
				Colors_Unknown4->container[loop] = &PlayerColorPointer->Unknown4;
				Colors_Unknown5->container[loop] = &PlayerColorPointer->Unknown5;
			}
		}

		Colors_ID->ChangeValue(lexical_cast<string>(PlayerColorPointer->ID));
		Colors_ColorL->ChangeValue(lexical_cast<string>(PlayerColorPointer->Colour));
		if(GameVersion < 2)	//	AoE and RoR
		{
			Colors_ColorL->SetBackgroundColour(wxColour(210, 230, 255));
			Colors_Name->ChangeValue(PlayerColorPointer->Name);
		}
		else	//	Above AoE and RoR
		{
			Colors_ColorL->SetBackgroundColour(wxColour(215, 255, 255));
			Colors_Palette->ChangeValue(lexical_cast<string>(PlayerColorPointer->Palette));
			Colors_MinimapColor->ChangeValue(lexical_cast<string>(PlayerColorPointer->MinimapColour));
			Colors_Unknown1->ChangeValue(lexical_cast<string>(PlayerColorPointer->Unknown1));
			Colors_Unknown2->ChangeValue(lexical_cast<string>(PlayerColorPointer->Unknown2));
			Colors_Unknown3->ChangeValue(lexical_cast<string>(PlayerColorPointer->Unknown3));
			Colors_Unknown4->ChangeValue(lexical_cast<string>(PlayerColorPointer->Unknown4));
			Colors_Unknown5->ChangeValue(lexical_cast<string>(PlayerColorPointer->Unknown5));
		}
	}
}

void AGE_Frame::OnPlayerColorsAdd(wxCommandEvent &Event)
{
	if(GenieFile != NULL)
	{
		wxBusyCursor WaitCursor;
		genie::PlayerColour Temp;
		Temp.setGameVersion(GenieVersion);
		GenieFile->PlayerColours.push_back(Temp);
		Added = true;
		ListPlayerColors();
	}
}

void AGE_Frame::OnPlayerColorsInsert(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		wxBusyCursor WaitCursor;
		genie::PlayerColour Temp;
		Temp.setGameVersion(GenieVersion);
		GenieFile->PlayerColours.insert(GenieFile->PlayerColours.begin() + ColorIDs[0], Temp);
		ListPlayerColors();
	}
}

void AGE_Frame::OnPlayerColorsDelete(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		wxBusyCursor WaitCursor;
		for(auto loop = Selections; loop--> 0;)
		GenieFile->PlayerColours.erase(GenieFile->PlayerColours.begin() + ColorIDs[loop]);
		ListPlayerColors();
	}
}

void AGE_Frame::OnPlayerColorsCopy(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		wxBusyCursor WaitCursor;
		copies->PlayerColor.resize(Selections);
		for(short loop=0; loop < Selections; loop++)
		copies->PlayerColor[loop] = GenieFile->PlayerColours[ColorIDs[loop]];
	}
}

void AGE_Frame::OnPlayerColorsPaste(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		wxBusyCursor WaitCursor;
		if(copies->PlayerColor.size()+ColorIDs[0] > GenieFile->PlayerColours.size())
		GenieFile->PlayerColours.resize(copies->PlayerColor.size()+ColorIDs[0]);
		for(short loop=0; loop < copies->PlayerColor.size(); loop++)
		{
			copies->PlayerColor[loop].setGameVersion(GenieVersion);
			GenieFile->PlayerColours[ColorIDs[0]+loop] = copies->PlayerColor[loop];
		}
		ListPlayerColors();
	}
}

void AGE_Frame::OnPlayerColorsPasteInsert(wxCommandEvent &Event)
{
	auto Selections = Colors_Colors_List->GetSelections(Items);
	if(Selections > 0)
	{
		wxBusyCursor WaitCursor;
		genie::PlayerColour Temp;
		GenieFile->PlayerColours.insert(GenieFile->PlayerColours.begin() + ColorIDs[0], copies->PlayerColor.size(), Temp);
		for(short loop=0; loop < copies->PlayerColor.size(); loop++)
		{
			copies->PlayerColor[loop].setGameVersion(GenieVersion);
			GenieFile->PlayerColours[ColorIDs[0]+loop] = copies->PlayerColor[loop];
		}
		ListPlayerColors();
	}
}

void AGE_Frame::CreatePlayerColorControls()
{
	Colors_Main = new wxBoxSizer(wxHORIZONTAL);
	Colors_ListArea = new wxBoxSizer(wxVERTICAL);
	Colors_Colors_Buttons = new wxGridSizer(3, 0, 0);
	Tab_PlayerColors = new wxPanel(TabBar_Main, wxID_ANY, wxDefaultPosition, wxSize(0, 20));

	Colors_Colors = new wxStaticBoxSizer(wxVERTICAL, Tab_PlayerColors, "Player Colors");
	Colors_Colors_Search = new wxTextCtrl(Tab_PlayerColors, wxID_ANY);
	Colors_Colors_Search_R = new wxTextCtrl(Tab_PlayerColors, wxID_ANY);
	Colors_Colors_List = new wxListBox(Tab_PlayerColors, wxID_ANY, wxDefaultPosition, wxSize(10, 100), 0, NULL, wxLB_EXTENDED);
	Colors_Add = new wxButton(Tab_PlayerColors, wxID_ANY, "Add", wxDefaultPosition, wxSize(5, 20));
	Colors_Insert = new wxButton(Tab_PlayerColors, wxID_ANY, "Insert", wxDefaultPosition, wxSize(5, 20));
	Colors_Delete = new wxButton(Tab_PlayerColors, wxID_ANY, "Delete", wxDefaultPosition, wxSize(5, 20));
	Colors_Copy = new wxButton(Tab_PlayerColors, wxID_ANY, "Copy", wxDefaultPosition, wxSize(5, 20));
	Colors_Paste = new wxButton(Tab_PlayerColors, wxID_ANY, "Paste", wxDefaultPosition, wxSize(5, 20));
	Colors_PasteInsert = new wxButton(Tab_PlayerColors, wxID_ANY, "PasteInsert", wxDefaultPosition, wxSize(5, 20));

	Colors_DataArea = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Name = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_ID = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Palette = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Color = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_MinimapColor = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_UnknownArea = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Unknown1 = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Unknown2 = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Unknown3 = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Unknown4 = new wxBoxSizer(wxVERTICAL);
	Colors_Holder_Unknown5 = new wxBoxSizer(wxVERTICAL);
	Colors_Text_Name = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Name", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_ID = new wxStaticText(Tab_PlayerColors, wxID_ANY, " ID", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Palette = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Palette", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Color = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Color", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_MinimapColor = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Minimap Color", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Unknown1 = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Unknown 1", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Unknown2 = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Unknown 2", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Unknown3 = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Unknown 3", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Unknown4 = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Unknown 4", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Text_Unknown5 = new wxStaticText(Tab_PlayerColors, wxID_ANY, " Unknown 5", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Colors_Name = new TextCtrl_String(Tab_PlayerColors, 30);
	Colors_ID = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Palette = new TextCtrl_Long(Tab_PlayerColors);
	Colors_ColorL = new TextCtrl_Long(Tab_PlayerColors);
	Colors_MinimapColor = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Unknown1 = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Unknown2 = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Unknown3 = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Unknown4 = new TextCtrl_Long(Tab_PlayerColors);
	Colors_Unknown5 = new TextCtrl_Long(Tab_PlayerColors);

	Colors_Colors_Buttons->Add(Colors_Add, 1, wxEXPAND);
	Colors_Colors_Buttons->Add(Colors_Insert, 1, wxEXPAND);
	Colors_Colors_Buttons->Add(Colors_Delete, 1, wxEXPAND);
	Colors_Colors_Buttons->Add(Colors_Copy, 1, wxEXPAND);
	Colors_Colors_Buttons->Add(Colors_Paste, 1, wxEXPAND);
	Colors_Colors_Buttons->Add(Colors_PasteInsert, 1, wxEXPAND);

	Colors_Colors->Add(Colors_Colors_Search, 0, wxEXPAND);
	Colors_Colors->Add(Colors_Colors_Search_R, 0, wxEXPAND);
	Colors_Colors->Add(-1, 2);
	Colors_Colors->Add(Colors_Colors_List, 1, wxEXPAND);
	Colors_Colors->Add(-1, 2);
	Colors_Colors->Add(Colors_Colors_Buttons, 0, wxEXPAND);

	Colors_ListArea->Add(-1, 10);
	Colors_ListArea->Add(Colors_Colors, 1, wxEXPAND);
	Colors_ListArea->Add(-1, 10);

	Colors_Holder_Name->Add(Colors_Text_Name, 0, wxEXPAND);
	Colors_Holder_Name->Add(Colors_Name, 1, wxEXPAND);
	Colors_Holder_ID->Add(Colors_Text_ID, 0, wxEXPAND);
	Colors_Holder_ID->Add(Colors_ID, 1, wxEXPAND);
	Colors_Holder_Palette->Add(Colors_Text_Palette, 0, wxEXPAND);
	Colors_Holder_Palette->Add(Colors_Palette, 1, wxEXPAND);
	Colors_Holder_Color->Add(Colors_Text_Color, 0, wxEXPAND);
	Colors_Holder_Color->Add(Colors_ColorL, 1, wxEXPAND);
	Colors_Holder_MinimapColor->Add(Colors_Text_MinimapColor, 0, wxEXPAND);
	Colors_Holder_MinimapColor->Add(Colors_MinimapColor, 1, wxEXPAND);
	Colors_Holder_Unknown1->Add(Colors_Text_Unknown1, 0, wxEXPAND);
	Colors_Holder_Unknown1->Add(Colors_Unknown1, 1, wxEXPAND);
	Colors_Holder_Unknown2->Add(Colors_Text_Unknown2, 0, wxEXPAND);
	Colors_Holder_Unknown2->Add(Colors_Unknown2, 1, wxEXPAND);
	Colors_Holder_Unknown3->Add(Colors_Text_Unknown3, 0, wxEXPAND);
	Colors_Holder_Unknown3->Add(Colors_Unknown3, 1, wxEXPAND);
	Colors_Holder_Unknown4->Add(Colors_Text_Unknown4, 0, wxEXPAND);
	Colors_Holder_Unknown4->Add(Colors_Unknown4, 1, wxEXPAND);
	Colors_Holder_Unknown5->Add(Colors_Text_Unknown5, 0, wxEXPAND);
	Colors_Holder_Unknown5->Add(Colors_Unknown5, 1, wxEXPAND);

	Colors_Holder_UnknownArea->Add(Colors_Holder_Unknown1, 0, wxEXPAND);
	Colors_Holder_UnknownArea->Add(-1, 5);
	Colors_Holder_UnknownArea->Add(Colors_Holder_Unknown2, 0, wxEXPAND);
	Colors_Holder_UnknownArea->Add(-1, 5);
	Colors_Holder_UnknownArea->Add(Colors_Holder_Unknown3, 0, wxEXPAND);
	Colors_Holder_UnknownArea->Add(-1, 5);
	Colors_Holder_UnknownArea->Add(Colors_Holder_Unknown4, 0, wxEXPAND);
	Colors_Holder_UnknownArea->Add(-1, 5);
	Colors_Holder_UnknownArea->Add(Colors_Holder_Unknown5, 0, wxEXPAND);

	Colors_DataArea->Add(-1, 10);
	Colors_DataArea->Add(Colors_Holder_Name, 0, wxEXPAND);
	Colors_DataArea->Add(-1, 5);
	Colors_DataArea->Add(Colors_Holder_ID, 0, wxEXPAND);
	Colors_DataArea->Add(-1, 5);
	Colors_DataArea->Add(Colors_Holder_Palette, 0, wxEXPAND);
	Colors_DataArea->Add(-1, 5);
	Colors_DataArea->Add(Colors_Holder_Color, 0, wxEXPAND);
	Colors_DataArea->Add(-1, 5);
	Colors_DataArea->Add(Colors_Holder_MinimapColor, 0, wxEXPAND);
	Colors_DataArea->Add(-1, 5);
	Colors_DataArea->Add(Colors_Holder_UnknownArea, 0, wxEXPAND);

	Colors_Main->Add(10, -1);
	Colors_Main->Add(Colors_ListArea, 1, wxEXPAND);
	Colors_Main->Add(10, -1);
//	Colors_Main->Add(-1, -1, 3, wxEXPAND);
	Colors_Main->Add(Colors_DataArea, 1, wxEXPAND);
	Colors_Main->AddStretchSpacer(2);
	Colors_Main->Add(10, -1);

	if(EnableIDFix)
	Colors_ID->Enable(false);

	Tab_PlayerColors->SetSizer(Colors_Main);

	Connect(Colors_Colors_Search->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsSearch));
	Connect(Colors_Colors_Search_R->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsSearch));
	Connect(Colors_Colors_List->GetId(), wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsSelect));
	Connect(Colors_Add->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsAdd));
	Connect(Colors_Insert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsInsert));
	Connect(Colors_Delete->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsDelete));
	Connect(Colors_Copy->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsCopy));
	Connect(Colors_Paste->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsPaste));
	Connect(Colors_PasteInsert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnPlayerColorsPasteInsert));
	Colors_Name->Connect(Colors_Name->GetId(), wxEVT_KILL_FOCUS, wxFocusEventHandler(AGE_Frame::OnKillFocus_Colors), NULL, this);
}

void AGE_Frame::OnKillFocus_Colors(wxFocusEvent &Event)
{
	if(((AGETextCtrl*)Event.GetEventObject())->SaveEdits())
	{
		ListPlayerColors();
	}
}
