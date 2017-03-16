#include "wx/wx.h"
#include "wx/fdrepdlg.h"
#include "CppSQLite3.h"

#include <string>	
#include <algorithm>
#include <stdlib.h>

#include "../wxlogo.xpm"

class ThePanel;
class TheFrame;

using namespace std;

class TheFrame : public wxFrame
{
public:
    TheFrame(const wxChar *title);
    ~TheFrame();
    void OnGoButton(wxCommandEvent& event);
    void OnCopyButton(wxCommandEvent& event);
    void OnSearchButton(wxCommandEvent& event);
    void OnIncFontSizeButton(wxCommandEvent& event);
    void OnDecFontSizeButton(wxCommandEvent& event);
    void OnCountButton(wxCommandEvent& event);

    void DisplayVerse(int bookindeks, int chapter, int startverse, int endverse);
    void SetSelection(long start, long end);

private:
    void BuildInterfaces();

protected:
    wxTextCtrl	*m_textBible;
    wxTextCtrl  *m_inputBible;
    wxButton    *m_btnGo;
    wxButton	*m_btnCopy;
    wxButton	*m_btnSearch;
    wxTextCtrl	*m_textToSearch;
    wxButton	*m_btnIncFontSize;
    wxButton	*m_btnDecFontSize;
    wxButton	*m_btnCount;

    int         m_fontSize;

    CppSQLite3DB m_db;

    DECLARE_EVENT_TABLE()
};



class BibleApp : public wxApp
{
public:
    virtual bool OnInit();

private:

};

IMPLEMENT_APP(BibleApp)

const int ID_BTN_GO = 210;
const int ID_BTN_COPY = 220;
const int ID_BTN_SEARCH = 230;
const int ID_BTN_INCFONTSIZE = 240;
const int ID_BTN_DECFONTSIZE = 250;
const int ID_BTN_COUNT = 260;


BEGIN_EVENT_TABLE(TheFrame, wxFrame)
EVT_BUTTON(ID_BTN_GO, TheFrame::OnGoButton)
EVT_BUTTON(ID_BTN_COPY, TheFrame::OnCopyButton)
EVT_BUTTON(ID_BTN_SEARCH, TheFrame::OnSearchButton)
EVT_BUTTON(ID_BTN_INCFONTSIZE, TheFrame::OnIncFontSizeButton)
EVT_BUTTON(ID_BTN_DECFONTSIZE, TheFrame::OnDecFontSizeButton)
EVT_BUTTON(ID_BTN_COUNT, TheFrame::OnCountButton)
END_EVENT_TABLE()
