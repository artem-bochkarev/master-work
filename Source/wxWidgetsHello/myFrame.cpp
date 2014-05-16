#include "myFrame.h"
#include "CGenerationViewer.h"
#include "AntShower.h"
#include <wx/sharedptr.h>

#include "GeneticCommon/ActionContainerImpl.h"
//#include "Genetic/CLaboratoryFactory.h"
#include "GeneticCommon/CleverAntMapFactory.hpp"

#include "CleverAnt3/CleverAnt3LaboratoryFactory.h"
#include "CleverAnt3/CleverAnt3Map.h"

#include "Tools/errorMsg.hpp"
#include "Tools/StringProcessor.h"

#include "TimeRun/CTimeCounter.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( _("Genetic laboratory"), wxPoint(50, 50), wxSize(650, 550) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, -1, title, pos, size), actions(0), broken(false)
{
    wxMenu* menuFile = new wxMenu();

	menuFile->Append( ID_Open, _("&Open"));
    menuFile->Append( ID_About, _("&About...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    wxMenu* menuTools = new wxMenu();
    menuTools->Append( ID_View, _("&View") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("&File") );
    menuBar->Append( menuTools, _("&Tools") );

    SetMenuBar( menuBar );

  //  CreateStatusBar();
  //  SetStatusText( _("Welcome to wxWidgets!") );


    wxBoxSizer* itemBoxSizerV = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizerV);

    button = new wxButton(this, wxID_OK, wxT("Start"),
        wxPoint(200, 200));


    itemStaticBitmap = new wxStaticBitmap( this, wxID_STATIC, wxNullBitmap, wxDefaultPosition, wxSize(500, 400), 0 );

	wxBoxSizer* textBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	infoLab = new wxStaticText(this, wxID_STATIC, _("0"), wxDefaultPosition, wxSize(300, 50), 0);
	infoLab->SetLabel("Load Laboratory file");
	infoPerf = new wxStaticText(this, wxID_STATIC, _("0"), wxDefaultPosition, wxSize(300, 50), 0);
	infoPerf->SetLabel("Performance info");
	textBoxSizer->Add(infoLab, 0, wxALIGN_LEFT);
	textBoxSizer->Add(infoPerf, 0, wxALIGN_RIGHT);

	itemBoxSizerV->Add(textBoxSizer, 0, wxEXPAND, 5);
    itemBoxSizerV->Add(itemStaticBitmap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemBoxSizerV->Add( button, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    this->Connect( ID_Quit, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnQuit) );
	this->Connect(ID_Open, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(MyFrame::OnOpen));
    this->Connect( ID_About, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnAbout) );

    this->Connect( ID_View, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnView) );
	timer.SetOwner(this, ID_Timer);
	this->Connect(ID_Timer, wxEVT_TIMER, wxTimerEventHandler(MyFrame::onTimer));
	this->Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MyFrame::onButton));
}

void MyFrame::initLaboratory(const std::string fName)
{
	try
	{
		/*laboratory = CLaboratoryFactory::getLaboratory(logger, fName);
		CMapPtr map1 = CMapFactory<CMapImpl>::getMap("map1.txt");
		//  CMap map2( 32, 32, 64 );
		CMapPtr map3 = CMapFactory<CMapImpl>::getRandomMap(32, 32, 64);

		maps.clear();
		maps.push_back(map1);
		//  maps.push_back( map2 );
		//  maps.push_back( map3 );
		laboratory->setMaps(maps);
		//laboratory->start();
		*/
		
		laboratory = CleverAnt3LaboratoryFactory::getLaboratory(logger, fName);
		CMapPtr map1 = CMapFactory<CleverAnt3Map>::getMap("map1.txt");
		maps.clear();
		maps.push_back(map1);
		laboratory->setMaps(maps);
		std::string text = laboratory->getStrategy()->getDeviceType();
		text.append(",");
		text.append(laboratory->getStrategy()->getPoolInfo());
		std::set<char> sepSet;
		sepSet.insert(',');
		sepSet.insert('.');
		infoLab->SetLabel(Tools::splitToManyLines(text, sepSet, 25));
		drawGraph();
		GetTimeManager().clean();
	}
	catch (std::exception& err)
	{
		Tools::printFailed(err.what(), &logger);
		broken = true;
		wxMessageBox(_(err.what()), _("Can't load config!"),
			wxOK | wxICON_INFORMATION, this);
	}
}

void MyFrame::showPerformance()
{
	std::stringstream out;
	bool bUseSeconds = false;
	for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
	{
		if (boost::chrono::duration_cast<boost::chrono::milliseconds>(val.second.duration) >= boost::chrono::milliseconds(10000))
		{
			bUseSeconds = true;
			break;
		}
	}

	for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
	{
		boost::chrono::microseconds mcs = boost::chrono::duration_cast<boost::chrono::microseconds>(val.second.duration);
		double speed = (double)val.second.counter / mcs.count();
		speed *= 1000000;
		int a = speed;
		int b = speed * 1000 - a * 1000;
		if (!bUseSeconds)
		{
			out << val.first << ": " << boost::chrono::duration_cast<boost::chrono::milliseconds>(val.second.duration) << "("
				<< a << "." << b << ")";
		}
		else
		{
			out << val.first << ": " << boost::chrono::duration_cast<boost::chrono::seconds>(val.second.duration) << "("
				<< a << "." << b << ")";
		}
	}
	std::string text = out.str();
	std::set<char> sepSet;
	sepSet.insert(',');
	sepSet.insert(')');
	infoPerf->SetLabel(Tools::splitToManyLines(text, sepSet, 30));
}

void MyFrame::free()
{
    timer.Stop();
	if (laboratory != 0)
	{
		laboratory->pause();
		laboratory = 0;
	}
    if ( actions != 0)
    {
        delete actions;
        actions = 0;
    }
}

void MyFrame::drawGraph()
{
    wxClientDC dc( itemStaticBitmap );
    wxSize size = itemStaticBitmap->GetClientSize();
    wxPen pen( wxColor( 255, 255, 255) , 1); // red pen of width 1
    wxBrush brush( wxColor( 255, 255, 255) );
    dc.SetBrush( brush );
    dc.SetPen( pen );
    dc.DrawRectangle( 0, 0, size.GetWidth(), size.GetHeight() );

    int n = laboratory->getGenerationNumber();
    int start = n - 100;
    if ( start < 0 )
        start = 0;
    int px = 0;
    int py = 0;
    wxPen penB( wxColor( 80, 80, 250 ), 2 );
    dc.SetPen( penB );
    for ( int i= start; i < n; ++i )
    {
        int x = (i - start)*4;
        double y = laboratory->getMaxFitness( i );
        y *= 4;
        dc.DrawLine( px, py, x, y );
        px = x;
        py = y;
    }
    px = py = 0;
    wxPen penG( wxColor( 80, 250, 80 ), 2 );
    dc.SetPen( penG );
    for ( int i= start; i < n; ++i )
    {
        int x = (i - start)*4;
        double y = laboratory->getAvgFitness( i );
        y *= 4;
        dc.DrawLine( px, py, x, y );
        px = x;
        py = y;
    }
}

void MyFrame::onTimer(wxTimerEvent& event)
{
	try
	{
		drawGraph();
	}
	catch (std::exception& ex)
	{
		Tools::printFailed(ex.what(), &logger);
		laboratory->pause();
		timer.Stop();
		broken = true;
		wxMessageBox(_(ex.what()), _("Update failed!"),
			wxOK | wxICON_INFORMATION, this);
	}
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    free();
    Close(true);
}

MyFrame::~MyFrame()
{
    free();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox( _("This is a Genetic laboratory"),
                  _("author Bochkarev Artem\n artem.bochkarev@gmail.com"), 
                  wxOK | wxICON_INFORMATION, this );
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog openFileDialog(this, _("Open config file"), "", "config.txt",
		"", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;
	broken = false;
	std::string fName = openFileDialog.GetPath();
	initLaboratory(fName);
}

void MyFrame::OnView(wxCommandEvent& WXUNUSED(event))
{
	if (broken)
		return;
	if (laboratory == 0)
		return;
    GenerationViewer* viewDialog = new GenerationViewer( this );//, wxT("Ant Viewer"), wxPoint(100, 100), wxSize(600, 600) );
    //viewDialog->runAnt();
    viewDialog->setLaboratory( laboratory.get() );
    viewDialog->ShowModal();
}

void MyFrame::onButton(wxCommandEvent& event)
{
	if (broken)
		return;
	if (laboratory == 0)
		return;
    if ( laboratory->isRunning() )
    {
        laboratory->pause();
        button->SetLabel("Start");
		timer.Stop();
		showPerformance();
    }else
    {
        laboratory->start();
        button->SetLabel("Pause");
		timer.Start();
    }
}