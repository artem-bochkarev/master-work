#pragma once
#include "wx/wx.h"
#include "Genetic/GeneticParams.h"
#include "GeneticCommon/Test.hpp"
#include <wx/sharedptr.h>
#include <wx/timer.h>

class CAntViewer : public wxDialog
{
public:

    CAntViewer( wxWindow* parent, const wxString& title, const wxPoint& pos, 
		const wxSize& size, wxSharedPtr< CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> > test);
    void onTimer(wxTimerEvent& event);
    void drawMap();
    void drawAnt( wxColor color );

    void runAnt();

    //DECLARE_EVENT_TABLE()
private:
	wxSharedPtr< CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> > antArena;
    wxStaticText text;
    wxTimer timer;
    
    enum 
    {
        ID_Timer = 112
    };
};