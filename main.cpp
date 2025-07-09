#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>

using namespace Upp;

//====================================================================================
/**
    \class LogView
    \brief A robust, thread-safe RichEdit logger using direct inline QTF formatting.

    This component provides a simple API to log text with different styles (normal,
    bold, header, success, warning, error) to a read-only RichEdit control.
    It is designed to be thread-safe via the LogAsync method.
*/
//====================================================================================
struct LogView : RichEdit {
    //! The available logging styles.
    enum LogStyle { LOG_NORMAL, LOG_HEADER, LOG_BOLD, LOG_SUCCESS, LOG_WARNING, LOG_ERROR };

private:
    //! Template for normal text (12pt, default color).
    const char* TPL_NORMAL  = "[3 %s]";
    //! Template for bold text (12pt, bold, default color).
    const char* TPL_BOLD    = "[3* %s]";
    //! Template for a header (12pt, bold, in a boxed cell).
    const char* TPL_HEADER  = "{{ [3* %s]}}";
    //! Template for a success message (12pt, bold, green).
    const char* TPL_SUCCESS = "[3*@4 %s]";
    //! Template for a warning message (12pt, bold, orange).
    const char* TPL_WARNING = "[3*@(255.165.0) %s]";
    //! Template for an error message (12pt, bold, red).
    const char* TPL_ERROR   = "[3*@6 %s]";
  
    //! The internal buffer holding the entire QTF document.
    String content_buffer;
public:
    LogView() {
        SetReadOnly();
        NoRuler();
        ShowCodes(Null);
    }

    //! Logs a string of text with a specific style.
    void Log(const String& text, LogStyle style = LOG_NORMAL, bool newline = true) {
		const char* qtf_template;
        switch(style) {
            case LOG_BOLD:    qtf_template = TPL_BOLD;    break;
            case LOG_HEADER:  qtf_template = TPL_HEADER;  break;
            case LOG_SUCCESS: qtf_template = TPL_SUCCESS; break;
            case LOG_WARNING: qtf_template = TPL_WARNING; break;
            case LOG_ERROR:   qtf_template = TPL_ERROR;   break;
            default:          qtf_template = TPL_NORMAL;  break;
        }

        content_buffer.Cat(Format(qtf_template, DeQtf(text)));
  
        if (newline) {
            content_buffer.Cat('&');
        }

        SetQTF(content_buffer);
        Move(GetLength());
        ScrollToCursor();
    }
    //! Adds a simple horizontal line to the log (cant find better way)
    void AddSeparator() {
        content_buffer.Cat("[1 _____________________________]&");
        SetQTF(content_buffer);
        Move(GetLength());
        ScrollToCursor();
    }
    
    //! Posts a logging request to the main GUI thread.
    void LogAsync(const String& text, LogStyle style = LOG_NORMAL, bool newline = true) {
        PostCallback([=] { Log(text, style, newline); });
    }
};


//====================================================================================
// ExampleRunner: Demonstrates the LogView component.
//====================================================================================
struct ExampleRunner : TopWindow {
    typedef ExampleRunner CLASSNAME;
	
    LogView logDisplay;
    Button  actionButton;
    bool    tasks_cancelled = false;

	ExampleRunner() {
	    Title("LogView Example").Sizeable().Zoomable();
	    SetRect(0, 0, 600, 300);
	
	    Add(logDisplay.VSizePos(0, 40).HSizePos());
	    Add(actionButton.BottomPos(4, 30).HSizePos(4, 4));
	
	    actionButton.SetLabel("Cancel");

	    actionButton <<= callback(this, &ExampleRunner::CancelTasks);
	
	    PostCallback([this] { RunAllTasks(); });
	}

    void CancelTasks() {
        tasks_cancelled = true;
        actionButton.Disable();
        actionButton.SetLabel("Cancelling...");
    }

    void RunAllTasks() {
        logDisplay.Log("Tasks Started.", LogView::LOG_HEADER);
        RunTask("Task A:", 150, true);
        RunTask("Task B:", 150, true);
        RunTask("Task C:", 150, false, true); // This one will show a warning
        RunTask("Failing Task:", 150, false);

        if (tasks_cancelled) {
            logDisplay.Log("Tasks Cancelled By User", LogView::LOG_ERROR);
        } else {
            logDisplay.Log("All tasks finished.", LogView::LOG_HEADER);
        }

        actionButton.SetLabel("Close");
        actionButton.Enable();
        actionButton <<= callback(this, &TopWindow::Close);
    }

    void RunTask(const String& name, int delay_ms, bool pass, bool with_warning = false) {
        if (tasks_cancelled) return;

        logDisplay.Log(name, LogView::LOG_NORMAL, false);
        logDisplay.Log(" Performing some work...", LogView::LOG_NORMAL, false);
        Sleep(delay_ms);

        if (with_warning) {
             logDisplay.Log(" -> WARNING", LogView::LOG_WARNING);
        } else if (pass) {
            logDisplay.Log(" -> PASSED", LogView::LOG_SUCCESS);
        } else {
            logDisplay.Log(" -> FAILED", LogView::LOG_ERROR);
        }
    }
};

// Entry point
GUI_APP_MAIN {
    StdLogSetup(LOG_COUT);
    ExampleRunner().Run();
}