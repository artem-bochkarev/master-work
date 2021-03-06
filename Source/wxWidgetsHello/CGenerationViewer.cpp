// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "AntShower.h"
#include <wx/sharedptr.h>
#include "CGenerationViewer.h"

////@begin XPM images
////@end XPM images


/*
 * GenerationViewer type definition
 */

IMPLEMENT_DYNAMIC_CLASS( GenerationViewer, wxDialog )


/*
 * GenerationViewer event table definition
 */

BEGIN_EVENT_TABLE( GenerationViewer, wxDialog )

////@begin GenerationViewer event table entries
EVT_BUTTON( ID_BUTTON1, GenerationViewer::OnButton1Click )
////@end GenerationViewer event table entries

END_EVENT_TABLE()


/*
 * GenerationViewer constructors
 */

GenerationViewer::GenerationViewer()
{
    Init();
}

GenerationViewer::GenerationViewer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/*
 * GenerationViewer creator
 */

bool GenerationViewer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin GenerationViewer creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end GenerationViewer creation
    return true;
}


/*
 * GenerationViewer destructor
 */

GenerationViewer::~GenerationViewer()
{
////@begin GenerationViewer destruction
////@end GenerationViewer destruction
}


/*
 * Member initialisation
 */

void GenerationViewer::Init()
{
////@begin GenerationViewer member initialisation
////@end GenerationViewer member initialisation
}


/*
 * Control creation for GenerationViewer
 */

void GenerationViewer::CreateControls()
{    
////@begin GenerationViewer content construction
    // Generated by DialogBlocks, 23/11/2011 22:55:28 (unregistered)

    GenerationViewer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    itemListCtrl = new wxListCtrl( itemDialog1, ID_LISTCTRL, wxDefaultPosition, wxSize(200, 200), wxLC_REPORT|wxLC_SINGLE_SEL );
    itemBoxSizer2->Add(itemListCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxGROW, 5);

    wxListItem col0; 
	col0.SetId(0); 
	col0.SetText( _("Generation") ); 
	col0.SetWidth(100);
	itemListCtrl->InsertColumn(0, col0); 
 
	// Add second column 
	wxListItem col1; 
	col1.SetId(1); 
	col1.SetText( _("MAX") );
	col1.SetWidth(50);
	itemListCtrl->InsertColumn(1, col1); 

    wxListItem col2; 
	col2.SetId(2); 
	col2.SetText( _("AVG") );
	col2.SetWidth(50);
	itemListCtrl->InsertColumn(2, col2);

    int k = itemListCtrl->GetColumnCount();

    wxButton* itemButton4 = new wxButton( itemDialog1, ID_BUTTON1, _("View"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end GenerationViewer content construction
}


/*
 * Should we show tooltips?
 */

bool GenerationViewer::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap GenerationViewer::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin GenerationViewer bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end GenerationViewer bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon GenerationViewer::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin GenerationViewer icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end GenerationViewer icon retrieval
}

void GenerationViewer::setLaboratory(CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>* lab)
{
    laboratory = lab;
    for ( size_t i=0; i < lab->getGenerationNumber(); ++i )
    {
        long item = itemListCtrl->InsertItem( 0, wxString::Format( wxT("%i"), i ) + _(" Generation") );
        itemListCtrl->SetItem( item, 1, wxString::Format( wxT("%.0f"), lab->getMaxFitness(i) ) );
        itemListCtrl->SetItem( item, 2, wxString::Format( wxT("%.2f"), lab->getAvgFitness(i) ) );
    }
}

void GenerationViewer::OnButton1Click( wxCommandEvent& event )
{
    long item = itemListCtrl->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    if ( item != -1 )
    {
        CMapPtr arena( laboratory->getMap( 0 ) );
        CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat = laboratory->getBestInd( laboratory->getGenerationNumber() - item - 1 );
		
		wxSharedPtr< CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> > 
			antTester(new CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>(
				automat, arena.get(), laboratory->getFitnesFunctor()->steps())
			);
        wxSharedPtr<AntViewer> viewDialog( new AntViewer( this ) );
		viewDialog->setTester(antTester);
		viewDialog->ShowModal();
    }
}