#pragma once
#include "wx/wx.h"
#include "Genetic/CTest.h"
#include <wx/sharedptr.h>
#include <wx/timer.h>

class CAntViewer : public wxDialog
{
public:

    CAntViewer( wxWindow* parent, const wxString& title, const wxPoint& pos, 
        const wxSize& size, wxSharedPtr<CTest> test );
    void onTimer(wxTimerEvent& event);
    void drawMap();
    void drawAnt( wxColor color );

    void runAnt();

    //DECLARE_EVENT_TABLE()
private:
    wxSharedPtr<CTest> antArena;
    wxStaticText text;
    wxTimer timer;
    
    enum 
    {
        ID_Timer = 112
    };
};