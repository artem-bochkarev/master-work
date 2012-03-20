#ifndef _GENERATIONVIEWER_H_
#define _GENERATIONVIEWER_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/listctrl.h"
#include "GeneticAPI/CLaboratory.h"
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
#define ID_GENERATIONVIEWER 10004
#define ID_LISTCTRL 10005
#define ID_BUTTON1 10006
#define SYMBOL_GENERATIONVIEWER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_GENERATIONVIEWER_TITLE _("GenerationViewer")
#define SYMBOL_GENERATIONVIEWER_IDNAME ID_GENERATIONVIEWER
#define SYMBOL_GENERATIONVIEWER_SIZE wxSize(400, 300)
#define SYMBOL_GENERATIONVIEWER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * GenerationViewer class declaration
 */

class GenerationViewer: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( GenerationViewer )
    DECLARE_EVENT_TABLE()

    CLaboratory* laboratory;
    wxListCtrl* itemListCtrl;

public:
    /// Constructors
    GenerationViewer();
    GenerationViewer( wxWindow* parent, wxWindowID id = SYMBOL_GENERATIONVIEWER_IDNAME, const wxString& caption = SYMBOL_GENERATIONVIEWER_TITLE, const wxPoint& pos = SYMBOL_GENERATIONVIEWER_POSITION, const wxSize& size = SYMBOL_GENERATIONVIEWER_SIZE, long style = SYMBOL_GENERATIONVIEWER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_GENERATIONVIEWER_IDNAME, const wxString& caption = SYMBOL_GENERATIONVIEWER_TITLE, const wxPoint& pos = SYMBOL_GENERATIONVIEWER_POSITION, const wxSize& size = SYMBOL_GENERATIONVIEWER_SIZE, long style = SYMBOL_GENERATIONVIEWER_STYLE );

    /// Destructor
    ~GenerationViewer();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin GenerationViewer event handler declarations

////@end GenerationViewer event handler declarations

////@begin GenerationViewer member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end GenerationViewer member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
    void OnButton1Click( wxCommandEvent& event );


    void setLaboratory( CLaboratory* lab );

////@begin GenerationViewer member variables
////@end GenerationViewer member variables
};

#endif
    // _GENERATIONVIEWER_H_