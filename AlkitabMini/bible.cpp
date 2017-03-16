
#include "bible.h"

//Everything starts here. OnInit loads TheFrame.
bool BibleApp::OnInit()
{
	TheFrame *frame = new TheFrame(wxT("Alkitab Mini"));
	frame->Center();
	frame->Show(true);

	return true;
}

/*
//
// IMPLEMENTATTION FOR THEPANEL
//
*/

//We put everything in ThePanel such us button, listbox, etc
TheFrame::TheFrame(const wxChar *title)
    : wxFrame(NULL, -1, title, wxPoint(-1, -1), wxSize(600, 400))
{
    //Default font size
    m_fontSize = 12;

    //Open the database
    m_db.open("alkitab.db");
    
    BuildInterfaces();
    Center();
}

TheFrame::~TheFrame()
{
    //Close db.
    m_db.close();
}

void TheFrame::SetSelection(long start, long end)
{
    m_textBible->SetFocus();
    m_textBible->SetSelection(start, end);
}

void TheFrame::OnCopyButton(wxCommandEvent &event)
{
	m_textBible->SelectAll();
	m_textBible->Copy();
}

void TheFrame::OnGoButton(wxCommandEvent &event)
{
    if (!m_inputBible->IsEmpty()) {

        m_textBible->Clear();

        //We do parsing here
        CppSQLite3Query q = m_db.execQuery("select * from books order by 1;");

        //Remove leading space
        m_inputBible->SetValue(m_inputBible->GetValue().Trim(FALSE));

        size_t len = m_inputBible->GetValue().Len();
        size_t whitespace = m_inputBible->GetValue().find(' ');

        //Separate into left and right part, clean all whitespaces in the right part
        wxString left = m_inputBible->GetValue().Left(whitespace).MakeLower();
        wxString right = m_inputBible->GetValue().SubString(whitespace + 1, len);
        right.erase(remove_if(right.begin(), right.end(), isspace), right.end());

        //Get book indeks.
        int bookindeks = 0;
        while (!q.eof())
        {
            const char *ret = strstr(q.fieldValue("isi"), left);
            bookindeks++;
            if (ret) break;
            q.nextRow();
        }
        q.finalize(); //Avoid memory leakage

        size_t colon = right.find(':');
        size_t dash = right.find('-');
        len = right.Len();

        wxString chapter = right.substr(0, colon);
        wxString startverse = right.substr(colon + 1, dash - colon - 1);
        wxString endverse;

        if (colon > len) {// One whole chapter
            startverse = "0";
            endverse = "0";
        }
        else if (dash > colon && dash < len) // Multiple verses
            endverse = right.substr(dash + 1, len - dash - 1);
        else // One verse only
            endverse = startverse;

        //Put on display
        DisplayVerse(bookindeks, atoi(chapter.c_str()), atoi(startverse.c_str()), atoi(endverse.c_str()));
    }
}
void TheFrame::OnSearchButton(wxCommandEvent& event)
{
    if (m_textToSearch->GetValue().Len() == 0)
        return;

    size_t startindex = 0;
    size_t lentext = m_textBible->GetValue().Len();
    size_t lensearch = m_textToSearch->GetValue().Len();

    size_t f = m_textBible->GetValue().Freq('\n');
    m_textBible->SetStyle(0, lentext + f, wxTextAttr(*wxBLACK));
    
    while (startindex < lentext) {
        size_t n = m_textBible->GetValue().Lower().find(m_textToSearch->GetValue().Lower(), startindex);

        //After one verse, before the next verse, we have '\n'. This is not calculated by "find" function
        //Thus, we need to compensate. 
        f = m_textBible->GetValue().SubString(0, n).Freq('\n');
        
        if (n > lentext)
            break;
        m_textBible->SetStyle(n + f, n + lensearch + f, wxTextAttr(*wxGREEN));
        startindex = n + 1 + lensearch;
   }
}

void TheFrame::OnIncFontSizeButton(wxCommandEvent& event)
{
    if (m_fontSize < 20)
        m_fontSize = m_fontSize + 1;

    m_textBible->SetFont(wxFont(m_fontSize, wxDEFAULT, wxNORMAL, wxNORMAL));
}

void TheFrame::OnDecFontSizeButton(wxCommandEvent& event)
{
    if (m_fontSize > 8)
        m_fontSize = m_fontSize - 1;
   
    m_textBible->SetFont(wxFont(m_fontSize, wxDEFAULT, wxNORMAL, wxNORMAL));
}

void TheFrame::OnCountButton(wxCommandEvent& event)
{

}


void TheFrame::DisplayVerse(int bookindeks, int chapter, int startverse, int endverse)
{
    if (endverse < startverse)
        endverse = startverse;

    wxString command;
    wxString buf;

	//Dsiplay one chapter.
	if (startverse == 0 && endverse == 0)
        command.sprintf("select isi from bible where kitab = %i and pasal=%i;", bookindeks, chapter);
		
	//Display selection only.
	else{
		command.sprintf("select isi from bible where kitab=%i and pasal=%i and ( ayat=%i ", bookindeks, chapter, startverse);

		for (int i = startverse; i< endverse; i++){
			buf.sprintf("or ayat=%i ", i + 1); 
            command.append(buf);
		}
        command.append(")");
	}

	CppSQLite3Query q = m_db.execQuery(command);

	int i = startverse; // To display verse number.

    if (startverse == 0)
        i = 1;
    
    m_textBible->SetFont(wxFont(m_fontSize, wxDEFAULT, wxNORMAL, wxNORMAL));

    while (!q.eof())
	{
		buf.sprintf("[%i] ", i);
		m_textBible->AppendText(_T(buf));
		m_textBible->AppendText(_T(q.fieldValue("isi")));
		q.nextRow();
		m_textBible->AppendText(_T("\n\n"));
		i++;
	}
	q.finalize(); //Don't forget this or memory will leak.
}

void TheFrame::BuildInterfaces()
{
    m_textBible = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(600, 400), wxTE_MULTILINE | wxTE_RICH);
    m_inputBible = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(200, 20));
    m_btnGo = new wxButton(this, ID_BTN_GO, _T("GO"), wxDefaultPosition, wxSize(60, 20));
    m_btnCopy = new wxButton(this, ID_BTN_COPY, _T("Copy"), wxDefaultPosition, wxSize(60, 20));
    m_btnSearch = new wxButton(this, ID_BTN_SEARCH, _T("Search"), wxDefaultPosition, wxSize(60, 20));
    m_btnCount = new wxButton(this, ID_BTN_COUNT, _T("Count"), wxDefaultPosition, wxSize(60, 20));
    m_btnIncFontSize = new wxButton(this, ID_BTN_INCFONTSIZE, _T("++"), wxDefaultPosition, wxSize(30, 20));
    m_btnDecFontSize = new wxButton(this, ID_BTN_DECFONTSIZE, _T("--"), wxDefaultPosition, wxSize(30, 20));
    m_textToSearch = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(200, 20));

    // Grid sizer
    wxBoxSizer  *bs1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer  *bs2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer  *bs3 = new wxBoxSizer(wxHORIZONTAL);

    bs1->Add(m_inputBible);
    bs1->Add(m_btnGo);
    bs2->Add(m_textToSearch);
    bs2->Add(m_btnSearch);
    bs2->Add(m_btnCount);
    bs3->Add(m_btnIncFontSize);
    bs3->Add(m_btnDecFontSize);
   
    // Box Sizer
    wxBoxSizer *bs = new wxBoxSizer(wxVERTICAL);
    bs->Add(bs1);
    bs->Add(bs2);
    bs->Add(bs3);
    bs->Add(m_textBible, 1, wxALL | wxEXPAND);
    bs->Add(m_btnCopy);

    SetAutoLayout(TRUE);
    SetSizer(bs);

    //Set a little bit
    m_textBible->SetEditable(false);
    m_inputBible->SetFocus();
}