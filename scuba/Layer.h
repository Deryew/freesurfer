#ifndef Layer_h
#define Layer_h

#include "string_fixed.h"
#include <GL/gl.h>
#include <map>
#include "ViewState.h"
#include "IDTracker.h"
#include "DebugReporter.h"
#include "TclCommandManager.h"
#include "ScubaWindowToRASTranslator.h"
#include "ScubaToolState.h"
#include "InputState.h"
#include "Listener.h"
#include "Broadcaster.h"

class LayerStaticTclListener : public DebugReporter, 
			       public TclCommandListener {

  public :
    LayerStaticTclListener ();
    ~LayerStaticTclListener ();

    virtual TclCommandResult
      DoListenToTclCommand ( char* isCommand, int iArgc, char** iArgv );
};


class Layer : public DebugReporter, 
	      public IDTracker<Layer>,
	      public TclCommandListener,
	      public Listener,    // dataChanged
	      public Broadcaster  // layerChanged
{

  friend class ScubaViewTester;

 public:

  Layer();
  virtual ~Layer();

  void SetBytesPerPixel ( int icBytes ) { mBytesPerPixel = icBytes; }

  // Tell the layer to draw its contents into a GL frame buffer.
  virtual void DrawIntoBuffer( GLubyte* iBuffer, int iWidth, int iHeight,
			       ViewState& iViewState,
			       ScubaWindowToRASTranslator& iTranslator );
  
  // Asks the layer to describe a point of data by adding pairs of
  // labels and values.
  virtual void GetInfoAtRAS ( float iRAS[3],
			   std::map<std::string,std::string>& iLabelValues );
  
  // Should return a type description unique to the subclass.
  virtual std::string GetTypeDescription() { return "BaseLayer"; }

  virtual TclCommandResult
    DoListenToTclCommand ( char* isCommand, int iArgc, char** iasArgv );

  // Handle broadcast messages.
  virtual void
    DoListenToMessage ( std::string isMessage, void* iData );

  // Called when the layer's data is changed. Default behavior is to
  // request a redisplay.
  virtual void DataChanged ();

  void SetLabel( std::string isLabel ) { msLabel = isLabel; }
  std::string GetLabel() { return msLabel; }

  void SetOpacity( float iOpacity ) { mOpacity = iOpacity; }
  float GetOpacity() { return mOpacity; }

  void SetWidth( int iWidth );
  void SetHeight( int iHeight );

  virtual void HandleTool ( float iRAS[3], ViewState& iViewState,
			    ScubaWindowToRASTranslator& iTranslator,
			    ScubaToolState& iTool, InputState& iInput );

  // Redisplay posters.
  void RequestRedisplay() { mbPostRedisplay = true; }
  bool WantRedisplay() const { return mbPostRedisplay; }
  void RedisplayPosted() { mbPostRedisplay = false; }

  // Some drawing tools.
  void DrawPixelIntoBuffer ( GLubyte* iBuffer, int iWidth, int iHeight,
			     int iWindow[2], int iColor[3], float iOpacity );
  
  void DrawPixelIntoBuffer ( GLubyte* iAddress,
			     int iColor[3], float iOpacity );
  
  void DrawAALineIntoBuffer ( GLubyte* iBuffer, int iWidth, int iHeight,
			      int iFromWindow[2], int iToWindow[2],
			      int iColor[3], int iThickness, float iOpacity );
  void DrawLineIntoBuffer ( GLubyte* iBuffer, int iWidth, int iHeight,
			    int iFromWindow[2], int iToWindow[2],
			    int iColor[3], int iThickness, float iOpacity );

  virtual void GetPreferredInPlaneIncrements ( float oIncrements[3] );

 protected:

  int mWidth;
  int mHeight;

  std::string msLabel;
  
  float mOpacity;

  static LayerStaticTclListener mStaticListener;

  // Redisplay requested flag.
  bool mbPostRedisplay;

  int mBytesPerPixel;
};



#endif
