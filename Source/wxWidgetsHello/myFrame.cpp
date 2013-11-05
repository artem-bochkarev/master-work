#include "myFrame.h"
#include "CGenerationViewer.h"
#include "AntShower.h"
#include <wx/sharedptr.h>

#include "Genetic/CActionContainerImpl.h"
#include "Genetic/CStateContainerImpl.h"
#include "Genetic/CLaboratoryFactory.h"
#include "Genetic/CMapFactory.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( _("Hello World"), wxPoint(50, 50), wxSize(650, 550) );
    frame->Show(true);
    SetTopWindow(frame);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size), states(0), actions(0)
{
    wxMenu* menuFile = new wxMenu();

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
    itemBoxSizerV->Add(itemStaticBitmap, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    itemBoxSizerV->Add( button, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    this->Connect( ID_Quit, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnQuit) );
    this->Connect( ID_About, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnAbout) );

    this->Connect( ID_View, wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MyFrame::OnView) );

    try
    {
        laboratory = CLaboratoryFactory::getLaboratory( logger, "" );
    }catch( std::exception& err )
    {
        OutputDebugStringA( err.what() );
    }
    CMapPtr map1 = CMapFactory::getMap( "map1.txt" );
  //  CMap map2( 32, 32, 64 );
    CMapPtr map3 = CMapFactory::getRandomMap( 32, 32, 64 );
    
    maps.push_back( map1 );
  //  maps.push_back( map2 );
  //  maps.push_back( map3 );
    laboratory->setMaps( maps );
    //laboratory->start();

    drawGraph();
    timer.SetOwner( this, ID_Timer );
    this->Connect( ID_Timer, wxEVT_TIMER, wxTimerEventHandler( MyFrame::onTimer ) );
    this->Connect( wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MyFrame::onButton ) );
    timer.Start( 250 );

}

void MyFrame::free()
{
    timer.Stop();
    laboratory->pause();
    if ( actions != 0)
    {
        delete actions;
        actions = 0;
    }

    if ( states != 0)
    {
        delete states;
        states = 0;
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
    drawGraph();
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
    wxMessageBox( _("This is a wxWidgets Hello world sample"),
                  _("About Hello World"), 
                  wxOK | wxICON_INFORMATION, this );
}

void MyFrame::OnView(wxCommandEvent& WXUNUSED(event))
{
    GenerationViewer* viewDialog = new GenerationViewer( this );//, wxT("Ant Viewer"), wxPoint(100, 100), wxSize(600, 600) );
    //viewDialog->runAnt();
    viewDialog->setLaboratory( laboratory.get() );
    viewDialog->ShowModal();
}

void MyFrame::onButton(wxCommandEvent& event)
{
    if ( laboratory->isRunning() )
    {
        laboratory->pause();
        button->SetLabel("Start");
    }else
    {
        laboratory->start();
        button->SetLabel("Pause");
    }
}