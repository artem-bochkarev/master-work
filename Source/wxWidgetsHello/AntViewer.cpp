#include "AntViewer.h"

CAntViewer::CAntViewer( wxWindow* parent, const wxString &title, const wxPoint &pos, const wxSize &size, wxSharedPtr<CTestCL> test )
    :wxDialog( parent, -1, title, pos, size ), antArena( test )
{
   // text.Create( this, -1, wxT("0") );
    timer.SetOwner( this, ID_Timer );
    this->Connect( ID_Timer, wxEVT_TIMER, wxTimerEventHandler( CAntViewer::onTimer ) );
}

void CAntViewer::onTimer(wxTimerEvent &event)
{
    drawAnt( this->GetBackgroundColour() );
    if ( antArena->makeMove() )
    {
        drawMap();
        drawAnt( wxColor( 250, 150, 150 ) );
       // text.SetLabel( wxString::Format( wxT("%i"), antArena->movesMaked() ) );
    }else
    {
        timer.Stop();
    }
}

void CAntViewer::runAnt()
{
    drawMap();
    timer.Start( 500 );
}

void CAntViewer::drawMap()
{
    wxClientDC dc(this);
    wxSize size = this->GetClientSize();
    size_t width = antArena->getArena()->width();
    size_t height = antArena->getArena()->height();
    size_t dx = size.GetWidth() / width;
    size_t dy = size.GetHeight() / height;
    wxPen pen( this->GetBackgroundColour() , 1); // red pen of width 1
    wxBrush brush( this->GetBackgroundColour() );
    dc.SetPen(pen);
    dc.SetBrush( brush );
    dc.DrawRectangle(0, 0, dx*width, dy*height );

    pen.SetColour( wxColor(150, 150, 150) );
    dc.SetPen(pen);
    int x = 0, y = 0;
    for ( int i=0; i<=width; ++i)
    {
        dc.DrawLine( dx*i, 0, dx*i, dy*height );
    }
    for ( int i=0; i<=height; ++i)
    {
        dc.DrawLine( 0, dy*i, dx*width, dy*i );
    }
    pen.SetColour( wxColor(150, 150, 250) );
    brush.SetColour( wxColor(150, 150, 250) );
    dc.SetBrush( brush );
    for ( int i=0; i<width; ++i )
        for ( int j=0; j<height; ++j )
        {
            if ( antArena->getArena()->get( i, j ) > 0 )
            {
                dc.DrawCircle( dx*i + dx/2, dy*j + dy/2, (dx+dy)/4 );
            }
        }
    dc.SetPen(wxNullPen);
}

void CAntViewer::drawAnt( wxColor color )
{
    wxClientDC dc(this);
    wxSize size = this->GetClientSize();
    size_t width = antArena->getArena()->width();
    size_t height = antArena->getArena()->height();
    size_t dx = size.GetWidth() / width;
    size_t dy = size.GetHeight() / height;
    wxPen pen( color, 2); // red pen of width 1
    dc.SetPen(pen);

    int antX = antArena->getX();
    int antY = antArena->getY();
    EDirection dir = antArena->getDir();
    int x = dx * antX + dx/2;
    int y = dy * antY + dy/2;
    int k = dx/4;
    switch ( dir )
    {
    case(DLeft):
        dc.DrawLine( x-k, y, x+k, y+k );
        dc.DrawLine( x-k, y, x+k, y-k );
        break;
    case(DTop):
        dc.DrawLine( x, y-k, x+k, y+k );
        dc.DrawLine( x, y-k, x-k, y+k );
        break;
    case(DRight):
        dc.DrawLine( x+k, y, x-k, y+k );
        dc.DrawLine( x+k, y, x-k, y-k );    
        break;
    case(DBottom):
        dc.DrawLine( x, y+k, x+k, y-k );
        dc.DrawLine( x, y+k, x-k, y-k );   
        break;
    }
    dc.SetPen(wxNullPen);
}