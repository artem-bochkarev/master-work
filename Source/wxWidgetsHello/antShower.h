#ifndef _ANTVIEWER_H_
#define _ANTVIEWER_H_

#include "Genetic/CTest.h"
#include <wx/sharedptr.h>
#include <wx/timer.h>


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_ANTVIEWER 10000
#define ID_PANEL 10001
#define ID_BUTTON 10002
#define ID_CHECKBOX 10003
#define ID_TIMER 10004
#define SYMBOL_ANTVIEWER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_ANTVIEWER_TITLE _("AntViewer")
#define SYMBOL_ANTVIEWER_IDNAME ID_ANTVIEWER
#define SYMBOL_ANTVIEWER_SIZE wxSize(400, 300)
#define SYMBOL_ANTVIEWER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * AntViewer class declaration
 */

class AntViewer: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( AntViewer )
    DECLARE_EVENT_TABLE()

    wxSharedPtr<CTest> antArena;
    bool bMapDrawed;
    int buttonMode;

    void drawMap();
    void drawAnt( wxColor color );
    void drawVisibleCells( wxColor color );
    void drawFood( wxColor color );
    void onTimer(wxTimerEvent& event);
    void draw();

    wxPanel* itemPanel;
    wxCheckBox* itemCheckBox;
    wxButton* itemButton;
    wxStaticText* itemTextMoves;
    wxStaticText* itemTextEaten;

    wxTimer timer;

public:
    /// Constructors
    AntViewer();
    AntViewer( wxWindow* parent, wxWindowID id = SYMBOL_ANTVIEWER_IDNAME, const wxString& caption = SYMBOL_ANTVIEWER_TITLE, const wxPoint& pos = SYMBOL_ANTVIEWER_POSITION, const wxSize& size = SYMBOL_ANTVIEWER_SIZE, long style = SYMBOL_ANTVIEWER_STYLE );
	virtual int ShowModal();
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ANTVIEWER_IDNAME, const wxString& caption = SYMBOL_ANTVIEWER_TITLE, const wxPoint& pos = SYMBOL_ANTVIEWER_POSITION, const wxSize& size = SYMBOL_ANTVIEWER_SIZE, long style = SYMBOL_ANTVIEWER_STYLE );

    /// Destructor
    ~AntViewer();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin AntViewer event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnButtonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
    void OnCheckboxClick( wxCommandEvent& event );

////@end AntViewer event handler declarations

////@begin AntViewer member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end AntViewer member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin AntViewer member variables
////@end AntViewer member variables

    void setTester( wxSharedPtr<CTest> ptr );
};

#endif