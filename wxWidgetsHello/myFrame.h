#include "wx/wx.h"
#include "Genetic/CTest.h"
#include "Genetic/CLaboratoryMulti.h"

class MyApp: public wxApp
{
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame();
    void free();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnView(wxCommandEvent& event);

    //DECLARE_EVENT_TABLE()
private:
    void drawGraph();
    void onTimer(wxTimerEvent& event);
    void onButton(wxCommandEvent& event);

    wxStaticBitmap* itemStaticBitmap;
    wxButton* button;

    CStateContainer* states;
    CActionContainer* actions;
    CLaboratoryPtr laboratory;
    std::vector<CMapPtr> maps;
    wxTimer timer;
};

enum
{
    ID_Quit = 1,
    ID_About,
    ID_View,
    ID_Timer,
    ID_TEST
};

