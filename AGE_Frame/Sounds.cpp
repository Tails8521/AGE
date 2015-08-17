#include "../AGE_Frame.h"

string AGE_Frame::GetSoundName(int sound)
{
    if(dataset->Sounds[sound].Items.empty()) return "Empty";
    else if(dataset->Sounds[sound].Items.size() == 1)
    return "File: " + GetSoundItemName(0, sound) + " ";
    else
    return "Files: " + lexical_cast<string>(dataset->Sounds[sound].Items.size()) + " ";
}

void AGE_Frame::OnSoundsSearch(wxCommandEvent &event)
{
	How2List = SEARCH;
	ListSounds(false);
}

void AGE_Frame::ListSounds(bool all)
{
	InitSounds(all);
	wxTimerEvent E;
	OnSoundsTimer(E);
}

void AGE_Frame::InitSounds(bool all)
{
	searchText = Sounds_Sounds_Search->GetValue().Lower();
	excludeText = Sounds_Sounds_Search_R->GetValue().Lower();

	Sounds_Sounds_ListV->names.clear();
	Sounds_Sounds_ListV->indexes.clear();
	wxArrayString names;
	if(all) names.Alloc(dataset->Sounds.size());

	for(short loop = 0; loop < dataset->Sounds.size(); ++loop)
	{
		wxString Name = " "+FormatInt(loop)+" - "+GetSoundName(loop);
		if(SearchMatches(Name.Lower()))
		{
			Sounds_Sounds_ListV->names.Add(Name);
			Sounds_Sounds_ListV->indexes.push_back(loop);
		}
		if(all) names.Add(Name);
	}

	virtualListing(Sounds_Sounds_ListV);
	if(all) FillLists(SoundComboBoxList, names);
}

void AGE_Frame::OnSoundsSelect(wxCommandEvent &event)
{
    if(!soundTimer.IsRunning())
        soundTimer.Start(150);
}

void AGE_Frame::OnSoundsTimer(wxTimerEvent &event)
{
    soundTimer.Stop();
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
    wxBusyCursor WaitCursor;
    getSelectedItems(selections, Sounds_Sounds_ListV, SoundIDs);

	for(auto &box: uiGroupSound) box->clear();

	genie::Sound * SoundPointer;
	for(auto loop = selections; loop--> 0;)
	{
		SoundPointer = &dataset->Sounds[SoundIDs[loop]];

		Sounds_ID->prepend(&SoundPointer->ID);
		Sounds_Unknown1->prepend(&SoundPointer->Unknown1);
		if(GenieVersion >= genie::GV_TEST)
		Sounds_Unknown2->prepend(&SoundPointer->Unknown2);
	}
	SetStatusText("Selections: "+lexical_cast<string>(selections)+"    Selected sound: "+lexical_cast<string>(SoundIDs[0]), 0);

	for(auto &box: uiGroupSound) box->update();
	Sounds_ID->Enable(false);
	ListSoundItems();
}

void AGE_Frame::OnSoundsAdd(wxCommandEvent &event)
{
	if(NULL == dataset) return;

	wxBusyCursor WaitCursor;
	AddToListIDFix(dataset->Sounds);
	ListSounds();
}

void AGE_Frame::OnSoundsInsert(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	InsertToListIDFix(dataset->Sounds, SoundIDs[0]);
	ListSounds();
}

void AGE_Frame::OnSoundsDelete(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	DeleteFromListIDFix(dataset->Sounds, SoundIDs);
	ListSounds();
}

void AGE_Frame::OnSoundsCopy(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	CopyFromList(dataset->Sounds, SoundIDs, copies.Sound);
	Sounds_Sounds_ListV->SetFocus();
}

void AGE_Frame::OnSoundsPaste(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	if(Paste11)
	{
		if(Paste11Check(SoundIDs.size(), copies.Sound.size()))
		{
			PasteToListIDFix(dataset->Sounds, SoundIDs, copies.Sound);
		}
	}
	else
	{
		PasteToListIDFix(dataset->Sounds, SoundIDs[0], copies.Sound);
	}
	ListSounds();
}

void AGE_Frame::OnSoundsPasteInsert(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	PasteInsertToListIDFix(dataset->Sounds, SoundIDs[0], copies.Sound);
	ListSounds();
}

string AGE_Frame::GetSoundItemName(int item, int set)
{
	string Name = "";
	short Selection[2];
	for(short loop = 0; loop < 2; ++loop)
	Selection[loop] = Sounds_Items_SearchFilters[loop]->GetSelection();

	if(Selection[0] > 0)
	for(short loop = 0; loop < 2; ++loop)
	{
		switch(Selection[loop])
		{
			case 1: // DRS
				Name += "DRS "+lexical_cast<string>(dataset->Sounds[set].Items[item].ResourceID);
				break;
			case 2: // Probability
				Name += "P "+lexical_cast<string>(dataset->Sounds[set].Items[item].Probability);
				break;
			if(GenieVersion >= genie::GV_AoKA)
			{
			case 3: // Civilization
				Name += "C "+lexical_cast<string>(dataset->Sounds[set].Items[item].Civ);
				break;
			case 4: // Unknown
				Name += "U "+lexical_cast<string>(dataset->Sounds[set].Items[item].Unknown1);
				break;
			}
		}
		Name += ", ";
		if(Selection[1] < 1) break;
	}

	if(!dataset->Sounds[set].Items[item].FileName.empty())
	{
		Name += dataset->Sounds[set].Items[item].FileName;
	}
	else
	{
		Name += "NewFile.wav";
	}
	return Name;
}

void AGE_Frame::OnSoundItemsSearch(wxCommandEvent &event)
{
	How2List = SEARCH;
	ListSoundItems();
}

void AGE_Frame::ListSoundItems()
{
	searchText = Sounds_Items_Search->GetValue().Lower();
	excludeText = Sounds_Items_Search_R->GetValue().Lower();
	for(short loop = 0; loop < 2; ++loop)
	useAnd[loop] = Sounds_Items_UseAnd[loop]->GetValue();

	Sounds_Items_ListV->names.clear();
	Sounds_Items_ListV->indexes.clear();

	for(short loop = 0; loop < dataset->Sounds[SoundIDs[0]].Items.size(); ++loop)
	{
		wxString Name = " "+FormatInt(loop)+" - "+GetSoundItemName(loop, SoundIDs[0]);
		if(SearchMatches(Name.Lower()))
		{
			Sounds_Items_ListV->names.Add(Name);
			Sounds_Items_ListV->indexes.push_back(loop);
		}
	}
	virtualListing(Sounds_Items_ListV);

	for(short loop = 0; loop < 2; ++loop)
	useAnd[loop] = false;

	wxTimerEvent E;
	OnSoundItemsTimer(E);
}

void AGE_Frame::OnSoundItemsSelect(wxCommandEvent &event)
{
    if(!soundFileTimer.IsRunning())
        soundFileTimer.Start(150);
}

void AGE_Frame::OnSoundItemsTimer(wxTimerEvent &event)
{
    soundFileTimer.Stop();
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
    wxBusyCursor WaitCursor;
	for(auto &box: uiGroupSoundFile) box->clear();
	if(selections > 0)
	{
        getSelectedItems(selections, Sounds_Items_ListV, SoundItemIDs);

		genie::SoundItem * SoundItemPointer;
		for(auto loop = selections; loop--> 0;)
		{
			SoundItemPointer = &dataset->Sounds[SoundIDs[0]].Items[SoundItemIDs[loop]];

			SoundItems_Name->prepend(&SoundItemPointer->FileName);
			SoundItems_Resource->prepend(&SoundItemPointer->ResourceID);
			SoundItems_Probability->prepend(&SoundItemPointer->Probability);
			if(GenieVersion >= genie::GV_AoKA)
			{
				SoundItems_Civ->prepend(&SoundItemPointer->Civ);
				SoundItems_Unknown->prepend(&SoundItemPointer->Unknown1);
			}
		}
	}
	for(auto &box: uiGroupSoundFile) box->update();
}

void AGE_Frame::OnSoundItemsAdd(wxCommandEvent &event)
{
	auto selections = Sounds_Sounds_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	AddToList(dataset->Sounds[SoundIDs[0]].Items);
	ListSoundItems();
}

void AGE_Frame::OnSoundItemsInsert(wxCommandEvent &event)
{
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	InsertToList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs[0]);
	ListSoundItems();
}

void AGE_Frame::OnSoundItemsDelete(wxCommandEvent &event)
{
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	DeleteFromList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs);
	ListSoundItems();
}

void AGE_Frame::OnSoundItemsCopy(wxCommandEvent &event)
{
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	CopyFromList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs, copies.SoundItem);
	Sounds_Items_ListV->SetFocus();
}

void AGE_Frame::OnSoundItemsPaste(wxCommandEvent &event)
{
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	if(Paste11)
	{
		if(Paste11Check(SoundItemIDs.size(), copies.SoundItem.size()))
		{
			PasteToList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs, copies.SoundItem);
		}
	}
	else
	{
		PasteToList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs[0], copies.SoundItem);
	}
	ListSoundItems();
}

void AGE_Frame::OnSoundItemsPasteInsert(wxCommandEvent &event)
{
	auto selections = Sounds_Items_ListV->GetSelectedItemCount();
	if(selections < 1) return;

	wxBusyCursor WaitCursor;
	PasteInsertToList(dataset->Sounds[SoundIDs[0]].Items, SoundItemIDs[0], copies.SoundItem);
	ListSoundItems();
}

void AGE_Frame::OnSoundItemsCopyToSounds(wxCommandEvent &event)
{
	for(short loop=1; loop < SoundIDs.size(); ++loop)
	{
		dataset->Sounds[SoundIDs[loop]].Items = dataset->Sounds[SoundIDs[0]].Items;
	}
}

void AGE_Frame::LoadAllSoundFiles(wxCommandEvent &event)
{
	wxString Name;
	searchText = Sounds_AllItems_Search->GetValue().Lower();
	excludeText = Sounds_AllItems_Search_R->GetValue().Lower();
	for(short loop = 0; loop < 2; ++loop)
	useAnd[loop] = Sounds_AllItems_UseAnd[loop]->GetValue();

	Sounds_AllItems_ListV->names.clear();

	for(short sound = 0; sound < dataset->Sounds.size(); ++sound)
	{
		for(short file = 0; file < dataset->Sounds[sound].Items.size(); ++file)
		{
			Name = " S"+lexical_cast<string>(sound)+" F"+lexical_cast<string>(file)+" - "+GetSoundItemName(file, sound);
			if(SearchMatches(Name.Lower()))
			{
				Sounds_AllItems_ListV->names.Add(Name);
			}
		}
	}

    virtualListing(Sounds_AllItems_ListV);
	//Sounds_AllItems_ListV->SetFocus(); You need to check if searched or not.

	for(short loop = 0; loop < 2; ++loop)
	useAnd[loop] = false;

	wxTimerEvent E;
	OnAllSoundFileTimer(E);
}

void AGE_Frame::ClearAllSoundFiles(wxCommandEvent &event)
{
	Sounds_Sounds_Search->SetValue("");
	Sounds_Items_Search->SetValue("");
}

void AGE_Frame::OnAllSoundFileSelect(wxCommandEvent &event)
{
    if(!allSoundFilesTimer.IsRunning())
        allSoundFilesTimer.Start(150);
}

void AGE_Frame::OnAllSoundFileTimer(wxTimerEvent &event)
{
    allSoundFilesTimer.Stop();
	SearchAllSubVectors(Sounds_AllItems_ListV, Sounds_Sounds_Search, Sounds_Items_Search);
}

void AGE_Frame::CreateSoundControls()
{
	Sounds_Main = new wxBoxSizer(wxHORIZONTAL);
	Sounds_Sounds_Buttons = new wxGridSizer(3, 0, 0);
	Sounds_Items_Buttons = new wxGridSizer(3, 0, 0);
	Sounds_DataArea = new wxBoxSizer(wxVERTICAL);

	Tab_Sounds = new wxPanel(TabBar_Main, wxID_ANY, wxDefaultPosition, wxSize(0, 20));
	Sounds_Sounds = new wxStaticBoxSizer(wxVERTICAL, Tab_Sounds, "Sounds");
	Sounds_Sounds_Search = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_Sounds_Search_R = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_Sounds_ListV = new AGEListView(Tab_Sounds, wxSize(200, 100));
	Sounds_Add = new wxButton(Tab_Sounds, wxID_ANY, "Add", wxDefaultPosition, wxSize(5, 20));
	Sounds_Insert = new wxButton(Tab_Sounds, wxID_ANY, "Insert New", wxDefaultPosition, wxSize(5, 20));
	Sounds_Delete = new wxButton(Tab_Sounds, wxID_ANY, "Delete", wxDefaultPosition, wxSize(5, 20));
	Sounds_Copy = new wxButton(Tab_Sounds, wxID_ANY, "Copy", wxDefaultPosition, wxSize(5, 20));
	Sounds_Paste = new wxButton(Tab_Sounds, wxID_ANY, "Paste", wxDefaultPosition, wxSize(5, 20));
	Sounds_PasteInsert = new wxButton(Tab_Sounds, wxID_ANY, "Ins Copies", wxDefaultPosition, wxSize(5, 20));

	Sounds_Items = new wxStaticBoxSizer(wxVERTICAL, Tab_Sounds, "Sound Files");
	for(short loop = 0; loop < 2; ++loop)
	{
		Sounds_Items_Searches[loop] = new wxBoxSizer(wxHORIZONTAL);
		Sounds_Items_SearchFilters[loop] = new wxOwnerDrawnComboBox(Tab_Sounds, wxID_ANY, "", wxDefaultPosition, wxSize(0, 20), 0, NULL, wxCB_READONLY);
	}
	Sounds_Items_Search = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_Items_UseAnd[0] = new wxCheckBox(Tab_Sounds, wxID_ANY, "And", wxDefaultPosition, wxSize(40, 20));
	Sounds_Items_Search_R = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_Items_UseAnd[1] = new wxCheckBox(Tab_Sounds, wxID_ANY, "And", wxDefaultPosition, wxSize(40, 20));
	Sounds_Items_ListV = new AGEListView(Tab_Sounds, wxSize(200, 100));
	SoundItems_Add = new wxButton(Tab_Sounds, wxID_ANY, "Add", wxDefaultPosition, wxSize(5, 20));
	SoundItems_Insert = new wxButton(Tab_Sounds, wxID_ANY, "Insert New", wxDefaultPosition, wxSize(5, 20));
	SoundItems_Delete = new wxButton(Tab_Sounds, wxID_ANY, "Delete", wxDefaultPosition, wxSize(5, 20));
	SoundItems_Copy = new wxButton(Tab_Sounds, wxID_ANY, "Copy", wxDefaultPosition, wxSize(5, 20));
	SoundItems_Paste = new wxButton(Tab_Sounds, wxID_ANY, "Paste", wxDefaultPosition, wxSize(5, 20));
	SoundItems_PasteInsert = new wxButton(Tab_Sounds, wxID_ANY, "Ins Copies", wxDefaultPosition, wxSize(5, 20));
	SoundItems_CopyToSounds = new wxButton(Tab_Sounds, wxID_ANY, "Copy all to selected sounds", wxDefaultPosition, wxSize(5, 20));

	Sounds_ID_Holder = new wxBoxSizer(wxVERTICAL);
	Sounds_ID_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " Sound ID", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Sounds_ID = AGETextCtrl::init(CShort, &uiGroupSound, this, AGEwindow, Tab_Sounds);
	Sounds_Unknown1_Holder = new wxBoxSizer(wxVERTICAL);
	Sounds_Unknown1_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " Sound Unknown 1", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Sounds_Unknown1 = AGETextCtrl::init(CShort, &uiGroupSound, this, AGEwindow, Tab_Sounds);
	Sounds_Unknown2_Holder = new wxBoxSizer(wxVERTICAL);
	Sounds_Unknown2_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " Sound Unknown 2", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	Sounds_Unknown2 = AGETextCtrl::init(CLong, &uiGroupSound, this, AGEwindow, Tab_Sounds);

	SoundItems_Name_Holder = new wxBoxSizer(wxVERTICAL);
	SoundItems_Name_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " Filename", wxDefaultPosition, wxSize(200, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	SoundItems_Name = AGETextCtrl::init(CString, &uiGroupSoundFile, this, AGEwindow, Tab_Sounds);
	SoundItems_Resource_Holder = new wxBoxSizer(wxVERTICAL);
	SoundItems_Resource_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " File DRS Resource", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	SoundItems_Resource = AGETextCtrl::init(CLong, &uiGroupSoundFile, this, AGEwindow, Tab_Sounds);
	SoundItems_Probability_Holder = new wxBoxSizer(wxVERTICAL);
	SoundItems_Probability_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " File Probability", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	SoundItems_Probability = AGETextCtrl::init(CShort, &uiGroupSoundFile, this, AGEwindow, Tab_Sounds);
	SoundItems_Civ_Holder = new wxBoxSizer(wxVERTICAL);
	SoundItems_Civ_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " File Civilization", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	SoundItems_Civ = AGETextCtrl::init(CShort, &uiGroupSoundFile, this, AGEwindow, Tab_Sounds);
	SoundItems_Civ_ComboBox = new ComboBox_Plus1(Tab_Sounds, SoundItems_Civ);
	CivComboBoxList.push_front(SoundItems_Civ_ComboBox);
	SoundItems_Unknown_Holder = new wxBoxSizer(wxVERTICAL);
	SoundItems_Unknown_Text = new wxStaticText(Tab_Sounds, wxID_ANY, " File Unknown", wxDefaultPosition, wxSize(-1, 15), wxALIGN_LEFT | wxST_NO_AUTORESIZE);
	SoundItems_Unknown = AGETextCtrl::init(CShort, &uiGroupSoundFile, this, AGEwindow, Tab_Sounds);
    SoundFile_Play = new wxButton(Tab_Sounds, wxID_ANY, "Play WAV", wxDefaultPosition, wxSize(5, 20));

	Sounds_AllItems = new wxStaticBoxSizer(wxVERTICAL, Tab_Sounds, "Files of all Sounds");
	Sounds_AllItems_Searches[0] = new wxBoxSizer(wxHORIZONTAL);
	Sounds_AllItems_Searches[1] = new wxBoxSizer(wxHORIZONTAL);
	Sounds_AllItems_Search = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_AllItems_UseAnd[0] = new wxCheckBox(Tab_Sounds, wxID_ANY, "And", wxDefaultPosition, wxSize(40, 20));
	Sounds_AllItems_Search_R = new wxTextCtrl(Tab_Sounds, wxID_ANY);
	Sounds_AllItems_UseAnd[1] = new wxCheckBox(Tab_Sounds, wxID_ANY, "And", wxDefaultPosition, wxSize(40, 20));
	Sounds_AllItems_ListV = new AGEListView(Tab_Sounds, wxSize(200, 100));
	Sounds_AllItems_Buttons = new wxBoxSizer(wxHORIZONTAL);
	Sounds_AllItems_Load = new wxButton(Tab_Sounds, wxID_ANY, "Reload", wxDefaultPosition, wxSize(5, 20));
	Sounds_AllItems_Clear = new wxButton(Tab_Sounds, wxID_ANY, "Clear *", wxDefaultPosition, wxSize(5, 20));
	Sounds_AllItems_Clear->SetToolTip("Clear the modified search texts");

	Sounds_Sounds_Buttons->Add(Sounds_Add, 1, wxEXPAND);
	Sounds_Sounds_Buttons->Add(Sounds_Delete, 1, wxEXPAND);
	Sounds_Sounds_Buttons->Add(Sounds_Insert, 1, wxEXPAND);
	Sounds_Sounds_Buttons->Add(Sounds_Copy, 1, wxEXPAND);
	Sounds_Sounds_Buttons->Add(Sounds_Paste, 1, wxEXPAND);
	Sounds_Sounds_Buttons->Add(Sounds_PasteInsert, 1, wxEXPAND);

	Sounds_Sounds->Add(Sounds_Sounds_Search, 0, wxEXPAND);
	Sounds_Sounds->Add(Sounds_Sounds_Search_R, 0, wxEXPAND);
	Sounds_Sounds->Add(Sounds_Sounds_ListV, 1, wxEXPAND | wxBOTTOM | wxTOP, 2);
	Sounds_Sounds->Add(Sounds_Sounds_Buttons, 0, wxEXPAND);

	Sounds_Items_Buttons->Add(SoundItems_Add, 1, wxEXPAND);
	Sounds_Items_Buttons->Add(SoundItems_Delete, 1, wxEXPAND);
	Sounds_Items_Buttons->Add(SoundItems_Insert, 1, wxEXPAND);
	Sounds_Items_Buttons->Add(SoundItems_Copy, 1, wxEXPAND);
	Sounds_Items_Buttons->Add(SoundItems_Paste, 1, wxEXPAND);
	Sounds_Items_Buttons->Add(SoundItems_PasteInsert, 1, wxEXPAND);

	Sounds_Items_Searches[0]->Add(Sounds_Items_Search, 1, wxEXPAND);
	Sounds_Items_Searches[0]->Add(Sounds_Items_UseAnd[0], 0, wxEXPAND | wxLEFT, 2);
	Sounds_Items_Searches[1]->Add(Sounds_Items_Search_R, 1, wxEXPAND);
	Sounds_Items_Searches[1]->Add(Sounds_Items_UseAnd[1], 0, wxEXPAND | wxLEFT, 2);
	Sounds_Items->Add(Sounds_Items_Searches[0], 0, wxEXPAND);
	Sounds_Items->Add(Sounds_Items_Searches[1], 0, wxEXPAND);
	Sounds_Items->Add(Sounds_Items_SearchFilters[0], 0, wxEXPAND);
	Sounds_Items->Add(Sounds_Items_SearchFilters[1], 0, wxEXPAND);
	Sounds_Items->Add(Sounds_Items_ListV, 1, wxEXPAND | wxBOTTOM | wxTOP, 2);
	Sounds_Items->Add(Sounds_Items_Buttons, 0, wxEXPAND);
	Sounds_Items->Add(SoundItems_CopyToSounds, 0, wxEXPAND | wxTOP, 2);

	Sounds_ID_Holder->Add(Sounds_ID_Text, 0, wxEXPAND);
	Sounds_ID_Holder->Add(Sounds_ID, 1, wxEXPAND);
	Sounds_Unknown1_Holder->Add(Sounds_Unknown1_Text, 0, wxEXPAND);
	Sounds_Unknown1_Holder->Add(Sounds_Unknown1, 1, wxEXPAND);
	Sounds_Unknown2_Holder->Add(Sounds_Unknown2_Text, 0, wxEXPAND);
	Sounds_Unknown2_Holder->Add(Sounds_Unknown2, 1, wxEXPAND);
	SoundItems_Name_Holder->Add(SoundItems_Name_Text, 0, wxEXPAND);
	SoundItems_Name_Holder->Add(SoundItems_Name, 1, wxEXPAND);
	SoundItems_Resource_Holder->Add(SoundItems_Resource_Text, 0, wxEXPAND);
	SoundItems_Resource_Holder->Add(SoundItems_Resource, 1, wxEXPAND);
	SoundItems_Probability_Holder->Add(SoundItems_Probability_Text, 0, wxEXPAND);
	SoundItems_Probability_Holder->Add(SoundItems_Probability, 1, wxEXPAND);
	SoundItems_Civ_Holder->Add(SoundItems_Civ_Text, 0, wxEXPAND);
	SoundItems_Civ_Holder->Add(SoundItems_Civ, 1, wxEXPAND);
	SoundItems_Civ_Holder->Add(SoundItems_Civ_ComboBox, 1, wxEXPAND);
	SoundItems_Unknown_Holder->Add(SoundItems_Unknown_Text, 0, wxEXPAND);
	SoundItems_Unknown_Holder->Add(SoundItems_Unknown, 1, wxEXPAND);

	Sounds_AllItems_Searches[0]->Add(Sounds_AllItems_Search, 1, wxEXPAND);
	Sounds_AllItems_Searches[0]->Add(Sounds_AllItems_UseAnd[0], 0, wxEXPAND | wxLEFT, 2);
	Sounds_AllItems_Searches[1]->Add(Sounds_AllItems_Search_R, 1, wxEXPAND);
	Sounds_AllItems_Searches[1]->Add(Sounds_AllItems_UseAnd[1], 0, wxEXPAND | wxLEFT, 2);
	Sounds_AllItems->Add(Sounds_AllItems_Searches[0], 0, wxEXPAND);
	Sounds_AllItems->Add(Sounds_AllItems_Searches[1], 0, wxEXPAND);
	Sounds_AllItems->Add(Sounds_AllItems_ListV, 1, wxEXPAND | wxBOTTOM | wxTOP, 2);
	Sounds_AllItems_Buttons->Add(Sounds_AllItems_Load, 2, wxEXPAND | wxRIGHT, 2);
	Sounds_AllItems_Buttons->Add(Sounds_AllItems_Clear, 1, wxEXPAND);
	Sounds_AllItems->Add(Sounds_AllItems_Buttons, 0, wxEXPAND);

	Sounds_DataArea->Add(Sounds_ID_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(Sounds_Unknown1_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(Sounds_Unknown2_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundItems_Name_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundItems_Resource_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundItems_Probability_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundItems_Civ_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundItems_Unknown_Holder, 0, wxEXPAND | wxTOP, 5);
	Sounds_DataArea->Add(SoundFile_Play, 0, wxEXPAND | wxTOP, 5);

	Sounds_Main->Add(Sounds_Sounds, 1, wxEXPAND | wxALL, 5);
	Sounds_Main->Add(Sounds_Items, 1, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, 5);
	Sounds_Main->Add(Sounds_DataArea, 1, wxEXPAND | wxRIGHT, 5);
	Sounds_Main->Add(Sounds_AllItems, 1, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, 5);

	if(EnableIDFix)
	Sounds_ID->Enable(false);

	Tab_Sounds->SetSizer(Sounds_Main);

	for(short loop = 0; loop < 2; ++loop)
	{
		Connect(Sounds_Items_UseAnd[loop]->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSearch));
		Connect(Sounds_Items_SearchFilters[loop]->GetId(), wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(AGE_Frame::OnSelection_SearchFilters));
		Connect(Sounds_AllItems_UseAnd[loop]->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(AGE_Frame::LoadAllSoundFiles));
	}
	Connect(Sounds_Sounds_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(AGE_Frame::OnSoundsSelect));
	Connect(Sounds_Sounds_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(AGE_Frame::OnSoundsSelect));
	Connect(Sounds_Sounds_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_FOCUSED, wxCommandEventHandler(AGE_Frame::OnSoundsSelect));
	Connect(Sounds_Sounds_Search->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnSoundsSearch));
	Connect(Sounds_Sounds_Search_R->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnSoundsSearch));
	Connect(Sounds_Add->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsAdd));
	Connect(Sounds_Insert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsInsert));
	Connect(Sounds_Delete->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsDelete));
	Connect(Sounds_Copy->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsCopy));
	Connect(Sounds_Paste->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsPaste));
	Connect(Sounds_PasteInsert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundsPasteInsert));
	Connect(Sounds_Items_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSelect));
	Connect(Sounds_Items_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSelect));
	Connect(Sounds_Items_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_FOCUSED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSelect));
	Connect(Sounds_Items_Search->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSearch));
	Connect(Sounds_Items_Search_R->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::OnSoundItemsSearch));
	Connect(SoundItems_Add->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsAdd));
	Connect(SoundItems_Insert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsInsert));
	Connect(SoundItems_Delete->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsDelete));
	Connect(SoundItems_Copy->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsCopy));
	Connect(SoundItems_Paste->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsPaste));
	Connect(SoundItems_PasteInsert->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsPasteInsert));
	Connect(SoundItems_CopyToSounds->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::OnSoundItemsCopyToSounds));
	Connect(Sounds_AllItems_Search->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::LoadAllSoundFiles));
	Connect(Sounds_AllItems_Search_R->GetId(), wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(AGE_Frame::LoadAllSoundFiles));
	Connect(Sounds_AllItems_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(AGE_Frame::OnAllSoundFileSelect));
	Connect(Sounds_AllItems_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxCommandEventHandler(AGE_Frame::OnAllSoundFileSelect));
	Connect(Sounds_AllItems_ListV->GetId(), wxEVT_COMMAND_LIST_ITEM_FOCUSED, wxCommandEventHandler(AGE_Frame::OnAllSoundFileSelect));
	Connect(Sounds_AllItems_Load->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::LoadAllSoundFiles));
	Connect(Sounds_AllItems_Clear->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AGE_Frame::ClearAllSoundFiles));

    soundTimer.Connect(soundTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(AGE_Frame::OnSoundsTimer), NULL, this);
    soundFileTimer.Connect(soundFileTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(AGE_Frame::OnSoundItemsTimer), NULL, this);
    allSoundFilesTimer.Connect(allSoundFilesTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler(AGE_Frame::OnAllSoundFileTimer), NULL, this);
	SoundItems_Name->Connect(SoundItems_Name->GetId(), wxEVT_KILL_FOCUS, wxFocusEventHandler(AGE_Frame::OnKillFocus_Sounds), NULL, this);
}

void AGE_Frame::OnKillFocus_Sounds(wxFocusEvent &event)
{
	event.Skip();
	if(((AGETextCtrl*)event.GetEventObject())->SaveEdits() != 0) return;
	ListSoundItems();
}
