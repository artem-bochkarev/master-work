#include "wx/wx.h"
//#include "Genetic/GeneticParams.h"
#include "CleverAnt3/GeneticParams.h"
#include "GeneticCommon/Test.hpp"
#include "GeneticCommon/LaboratoryMulti.h"
#include "Tools/Logger.h"

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
	void OnOpen(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnView(wxCommandEvent& event);

    //DECLARE_EVENT_TABLE()
private:
    void drawGraph();
    void onTimer(wxTimerEvent& event);
    void onButton(wxCommandEvent& event);
	void initLaboratory(const std::string fName);
	void showPerformance();

    wxStaticBitmap* itemStaticBitmap;
    wxButton* button;
	wxStaticText* infoLab;
	wxStaticText* infoPerf;

	CActionContainer<COUNTERS_TYPE>* actions;
    CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> laboratory;
    std::vector<CMapPtr> maps;
    wxTimer timer;
    Tools::Logger logger;
	bool broken;
};

enum
{
    ID_Quit = 1,
    ID_About,
    ID_View,
    ID_Timer,
    ID_TEST,
	ID_Open
};

