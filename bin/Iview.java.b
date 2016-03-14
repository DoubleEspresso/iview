import java.util.concurrent.atomic.AtomicInteger;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseWheelListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.opengl.GLCanvas;
import org.eclipse.swt.opengl.GLData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.GLContext;
import com.jogamp.opengl.GLDrawableFactory;
import com.jogamp.opengl.glu.GLU;

// TODO: 
// 1. push rendering to background thread + remove flickering [x] --> removed some update calls + glclear in resize event (..still flickers when file browser opens..))
// 2. investigate memory leaks when loading images (each load creates new JPEG() .. never free pointer buffer to image data in cpp??)
// 2. update title of main window to loaded image location + image info display somewhere
// 2. double-click to re-zoom image
// 3. proper aspect ratios for loaded images + zoom support on mousewheel
// 4. add basic toolbar menu with file filter imops help etc.
// 5. generalize GLPane() class  -- goal to make it easier to attach a glpane to a generic swt window (in background thread)
// 6. settings file for initial screen resolution + start where left off --> need a .pstate file for applied filters, color tables etc.???
// --- long term TODO: ---
// 5. generalize image load class for types of images (include tiff, png, etc.)
// 5. advanced math support (fitting + libBlas + fftw(?) + others for image analysis tools) ==> cuda/gpu support
// 6. cpu histogram + equalization of images
// 7. gpu/cuda frame handler support 
// 8. filters class + ui support + sideview window for multiple images loaded (?) - or scrollbar or similar?
// 9. ability to draw on images/select regions etc.
// 10. add stack of do/redo operations
// 11. add split pane view + multiple image load support (drag & drop)
// 12. add support for learning algo for art + image

public class Iview
{
	public static int windowMinWidth = 320;
	public static int windowMinHeight = 240;
	public static float zoomf = 1.0f;
	public static float mouseX = 0;
	public static float mouseY = 0;
	public static boolean isDragging = false;
	public static float final_posX = 0;
	public static float final_posY = 0;
	public static int clickCount = 0;
	public static float dX = 0;
	public static float dY = 0;
	public static float cornerX = 0;
	public static float cornerY = 0;
	public static int displayWidth = 0;
	public static int displayHeight = 0;
	static AtomicInteger wheelRequest;
	
	public static void main(String[] args)
	{
		wheelRequest = new AtomicInteger();
        Display display = new Display();
        final Shell shell = new Shell( display );
        shell.setText( "Iview" );
        shell.setLayout( new FillLayout() );
        shell.setSize( 640, 480 );
        
        final Composite composite = new Composite( shell, SWT.NONE );
        composite.setLayout( new FillLayout() );
        
        GLData gldata = new GLData();
        gldata.doubleBuffer = true;
        
        // need SWT.NO_BACKGROUND to prevent SWT from clearing the window at the wrong times (we use glClear for this instead)
        final GLCanvas glcanvas = new GLCanvas( composite, SWT.NO_BACKGROUND, gldata );
        glcanvas.setCurrent();
        GLProfile glprofile = GLProfile.getDefault();
        
        final GLContext glcontext = GLDrawableFactory.getFactory( glprofile ).createExternalGLContext();
        initGL(glcontext);
       
        // event listeners for the main window
		{
			// resize event
			glcanvas.addListener(SWT.Resize, new Listener()
			{
				@Override
				public void handleEvent(Event event)
				{
					resizeMainWindow(glcanvas, glcontext);
				}
			});
			
			// key listener
			glcanvas.addKeyListener( new KeyListener()
			{
				@Override
				public void keyPressed(KeyEvent e)
				{
				}
				
				@Override
				public void keyReleased(KeyEvent e)
				{
					if (e.character == '=')
					{
						updateZoom(true, glcanvas, glcontext.getGL().getGL2());
					}
					else if (e.character == '-')
					{
						updateZoom(false, glcanvas, glcontext.getGL().getGL2());
					}
					System.out.println("released " + e.character);
				}
			});

			// mousewheel for zooming
			glcanvas.addMouseWheelListener(new MouseWheelListener() 
			{
				@Override
				public void mouseScrolled(MouseEvent e) 
				{
					if (wheelRequest.addAndGet(1) > 1) 
						{
							wheelRequest.decrementAndGet();
							return;
						}

					System.out.println(" nb request = " + wheelRequest.get() + " " + this);
					//glcanvas.setCurrent();
					//glcontext.makeCurrent();
					//updateZoom(e, glcanvas, glcontext.getGL().getGL2());
					//glcontext.release();
					try
					{
						Thread.sleep(2000);
					}
					catch (InterruptedException e1)
					{
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
					wheelRequest.decrementAndGet();
			    }
			 });

//			// double click to auto-scale
			glcanvas.addMouseListener(new MouseListener()
			{
				@Override
				public void mouseDown(MouseEvent e)
				{
//					++clickCount;
//					try
//					{
//						Thread.sleep(50);
//					}
//					catch (InterruptedException e1) { }
					
					 
					if (e.button == 1)// && clickCount == 1)
					{
						mouseX = e.x;
						mouseY = e.y;
						System.out.println("("+mouseX +","+mouseY+")");
						//isDragging = true;
						clickCount = 0;
						dX = (mouseX - dX);
						dY = (mouseY - dY);
					}
				}
				
				@Override
				public void mouseUp(MouseEvent e)
				{
					if (e.button == 1)
					{
						isDragging =  false;
						final_posX = (mouseX-dX); 
						final_posY = (mouseY-dY);
						//dX = final_posX;
						//dY = final_posY;
						
					}
				}
				
				@Override 
				public void mouseDoubleClick(MouseEvent e)
				{
					//Rectangle r = glcanvas.getClientArea();
					//autoScale(glcontext.getGL().getGL2(), r.width, r.height);
				}
								
			});
			// drag image for translate
			glcanvas.addListener(SWT.MouseMove, new Listener()
			{
				@Override
				public void handleEvent(Event event)
				{
					
					if (!isDragging) return;
					//mouseDX += event.x - mouseX;
					//mouseDY += event.y - mouseY;

					mouseX = event.x;
					mouseY = event.y;
					final_posX = mouseX;
					final_posY = mouseY;
					//imX = mouseX - imX;
					//imY = mouseY - imY;
				}
			});
			
			
			// TODO: drag & drop event
			
			// right-click menu for glcanvas
			attachRightClickMenu(glcontext, glcanvas);
			
			// paint events for the main window
			glcanvas.addPaintListener(new PaintListener()
			{
				@Override
				public void paintControl(PaintEvent paintevent)
				{
					renderMainWindow(glcanvas, glcontext);
				}
			});
		}
		
		shell.open();
		while (!shell.isDisposed())
		{
			if (!display.isDisposed()) glcanvas.redraw(); 
			if (!display.readAndDispatch()) display.sleep();
			
		}
		glcanvas.dispose();
		display.dispose();		
	}

	
	private static void autoScale(GL2 gl2, int width, int height)
	{
		if (FrameHandler.imageBuffer == null) return;
		zoomf = 1f;
		isDragging = false;
//		rescale = true;
//		
//    	// handle aspect ratio based on window size and image size
//    	float iw = (float) FrameHandler.image.width();
//    	float ih = (float) FrameHandler.image.height();       	
//    	float s = (float)Math.min(1, (float) width / iw);
//    	float t = (float)Math.min(1, (float) height / ih);
//
//    	float w = s * iw; 
//    	float h = t * ih;
//    	if (s!=t) 
//    		{
//    			if (t < s) w = t * ih * FrameHandler.aspect;
//    			else h = s * iw / (FrameHandler.aspect);
//    		}
//    	float dX = (float) ((width - w) * 0.5);
//    	float dY = (float) ((height - h) * 0.5);
//    	gl2.glTranslatef(dX, dY, 0f);
	}
	
	private static void initGL(GLContext glcontext)
	{
		GL gl = glcontext.getGL();
		gl.glEnable(GL.GL_TEXTURE_2D);
		gl.glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	}

	private static void updateZoom(Boolean zoomin, GLCanvas glcanvas, GL2 gl2)
	{			
		//mouseX = e.x;
		//mouseY = e.y;
		
		//if (e.count < 0) zoomf += 0.02f;
		//else zoomf -= 0.02f;
		if (zoomin) zoomf += 0.9f;
		else zoomf -= 0.9f;
		
		Point mLoc = Display.getCurrent().getCursorLocation();
		Point mPoint = Display.getCurrent().getFocusControl().toControl(mLoc);
		
		
//		if (e.count < 0) zoomf -= 0.06;
//		else zoomf += 0.06;
//		
		//if (zoomf < 0f) zoomf = 0f;
		
		Rectangle r = glcanvas.getClientArea();
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        gl2.glLoadIdentity();                     


    	float iw = (float) FrameHandler.image.width();
    	float ih = (float) FrameHandler.image.height();       	
    	float s = (float)Math.min(1, (float) r.width / iw);
    	float t = (float)Math.min(1, (float) r.height / ih);

    	float w = s * iw; 
    	float h = t * ih;
    	if (s!=t) 
    		{
    			if (t < s) w = t * ih * FrameHandler.aspect;
    			else h = s * iw / (FrameHandler.aspect);
    		}
    

        
    // mouse independent zoom

    float mX = (float) 2f*mPoint.x / (float) w-1f; // in [-1,1] on image
    float mY = (float) 2f*mPoint.y / (float) h-1f;
		if (mY > 1) mY = 1;
		if (mY < -1) mY = -1;
		if (mX > 1) mX = 1;
		if (mX < -1) mX = -1;    
    
    float cX = (float) w / 2f + w * mX / 2f; 
    float cY = (float) h / 2f + h * mY / 2f;


//
//    float sr = 1f;
//    float sl = 1f;
//    float su = 1f;
//    float sd = 1f;
    
//    if (mX <= 0.5) { sl = mX; sr = (1f-mX);}
//    else { sl=(1-mX); sr=mX; }
//    
//    if (mY <= 0.5) { sd = mY; su = (1-mY); }
//    else { sd = (1-mY); su = mY; }
    
    gl2.glOrtho((cX - cX / zoomf), 
    			(cX + cX / zoomf), 
    			(cY + cY / zoomf), 
    			(cY - cY / zoomf), 
    			0f, 1f);

  // too zoom on bottom right corner [1,1] 
//  gl2.glOrtho((cX - cX / zoomf), 
//	(float)w, 
//	(float)h, 
//	(cY - cY / zoomf), 
//	0f, 1f);
 
 // too zoom on top left corner  [0,0]
//  gl2.glOrtho(0f, 
//	(float)(cX + cX / zoomf), 
//	(float)(cY + cY / zoomf), 
//	0f, 
//	0f, 1f);
    
  // general?
//gl2.glOrtho((cX - cX / zoomf)*mX, 
//	w*mX + (1-mX) * (float)(cX + cX / zoomf), 
//	h*mY + (1-mY) * (float)(cY + cY / zoomf), 
//	(cY - cY / zoomf)*mY, 
//	0f, 1f); 
    
    	cornerX = -(cX - cX / zoomf); 
    	cornerY = -(cY - cY / zoomf); 
    	System.out.println("cx = " + cornerX + " cy = " + cornerY + " dw = " + displayWidth + " dh = " + displayHeight + " zoomf = " + zoomf);
    	System.out.println("mX = " + mX + " my = " + mY);
    
        
      gl2.glMatrixMode(GL2.GL_MODELVIEW);
	}
	
	private static void resizeMainWindow(GLCanvas glcanvas, GLContext glcontext)
	{
		Rectangle rectangle = glcanvas.getClientArea();
		if (rectangle.width < windowMinWidth)
		{
			glcanvas.setSize(windowMinWidth, rectangle.height);
			return;
		}
		else if (rectangle.height < windowMinHeight)
		{
			glcanvas.setSize(rectangle.width, windowMinHeight);
			return;
		}
		glcanvas.setCurrent();
		glcontext.makeCurrent();
		updateWindow(glcontext.getGL().getGL2(), rectangle.width, rectangle.height);
		glcontext.release();
	}
	private static void renderMainWindow(final GLCanvas glcanvas, final GLContext glcontext)
	{
		// rendering is done in an async thread
		Display.getDefault().asyncExec(new Runnable()
		{
			public void run()
			{
				if (!glcanvas.isDisposed()) // avoid errors on exit
				{
					Rectangle rectangle = glcanvas.getClientArea();
					glcanvas.setCurrent();
					glcontext.makeCurrent();
					paintCanvas(glcontext.getGL().getGL2(), rectangle.width, rectangle.height);
					glcanvas.swapBuffers();
					glcontext.release();
				}
			}
		});
	}
	
    protected static void updateWindow( GL2 gl2, int width, int height ) {
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();
               
        //gl2.glOrtho(0, width, height, 0, -1, 1);
        gl2.glOrtho(0, height, width, 0, 0f, 1f);
        gl2.glViewport( 0, 0, width, height);
        gl2.glMatrixMode(GL2.GL_MODELVIEW);
    }
    
	protected static void paintCanvas(GL2 gl2, int width, int height )
	{
        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
        gl2.glLoadIdentity();

        
        if (FrameHandler.imageBuffer != null) 
        {
        	gl2.glDisable(GL2.GL_CULL_FACE);
        	gl2.glDisable(GL2.GL_LIGHTING);
        	gl2.glDisable(GL2.GL_DEPTH_TEST);
        	gl2.glEnable(GL2.GL_TEXTURE_2D);
 
        	// handle aspect ratio based on window size and image size
        	float iw = (float) FrameHandler.image.width();
        	float ih = (float) FrameHandler.image.height();       	
        	float s = (float)Math.min(1, (float) width / iw);
        	float t = (float)Math.min(1, (float) height / ih);

        	float w = s * iw; 
        	float h = t * ih;
        	if (s!=t) 
        		{
        			if (t < s) w = t * ih * FrameHandler.aspect;
        			else h = s * iw / (FrameHandler.aspect);
        		}

        	displayWidth  = (int) (w * zoomf);
        	displayHeight = (int) (h * zoomf);
        	
        	// save current matrix
        	if (zoomf == 1)
        	{
        	gl2.glPushMatrix();
        	gl2.glMatrixMode( GL2.GL_PROJECTION );
        	gl2.glLoadIdentity();
            gl2.glOrtho(0, w, h, 0, 0f, 1f);
            gl2.glViewport( 0, 0, (int)w, (int)h);
            gl2.glMatrixMode(GL2.GL_MODELVIEW);
            gl2.glPopMatrix();
        	}
            
			// magnification
			if (zoomf != 1 )// && !isDragging)
			{
				//gl2.glTranslatef(mouseX, mouseY, 0f);
				//gl2.glScalef(zoomf, zoomf, 0f);
				//gl2.glTranslatef(-mouseX, -mouseY, 0f);

				//glu.gluPerspective(45f, (float) width / (float) height, 0.1f, 100f);

			}

//			if (isDragging)
//			{
//				gl2.glScalef(1f, 1f, 1f);
//				gl2.glTranslatef(mouseX - dX, mouseY - dY, 0f);
//				//gl2.glScalef(zoomf, zoomf, 1f);
//
//			}
//			else
//			{
//				gl2.glScalef(1f, 1f, 1f);
//				gl2.glTranslatef(final_posX, final_posY, 0f);
//				//gl2.glScalef(zoomf, zoomf, 1f);
//			}

        	// move to auto-scale -- only on load or rescale
//        	float dX = (float) ((width - w) * 0.5);
//        	float dY = (float) ((height - h) * 0.5);
//       		gl2.glTranslatef(dX, dY, 0f);

        	gl2.glBindTexture(GL2.GL_TEXTURE_2D, FrameHandler.texture_id);       	
        	gl2.glBegin(GL2.GL_QUADS);
        	gl2.glTexCoord2f(0, 0); gl2.glVertex2d(0, 0);
        	gl2.glTexCoord2f(1, 0); gl2.glVertex2d(w,  0);
        	gl2.glTexCoord2f(1, 1); gl2.glVertex2d(w, h);
        	gl2.glTexCoord2f(0, 1); gl2.glVertex2d(0, h);
        	gl2.glEnd();        	

        	gl2.glDisable(GL2.GL_TEXTURE_2D);
        	
        }
	}

	private static void attachRightClickMenu(final GLContext glcontext, final GLCanvas glcanvas)
	{
		// new file selection
		Menu popupMenu = new Menu(glcanvas);
		MenuItem fileOpen = new MenuItem(popupMenu, SWT.CASCADE);
		fileOpen.setText("&File");
		final Shell browserShell = new Shell(glcanvas.getDisplay());
	
		fileOpen.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				FileDialog dialog = new FileDialog(browserShell, SWT.MULTI);
				// dialog.setFilterExtensions(new String [] {"*.jpeg"});
				// dialog.setFilterPath("c:\\temp");
				String basename = StringUtils.basename(dialog.open());
				if (basename != null)
				{
					String[] files = dialog.getFileNames();
					for (int i=0; i<files.length; ++i) 
					{
						files[i] = basename + StringUtils.dirSeparator() + files[i]; // full pathname	
						System.out.println("..selected (" + files[i] + ")");
						
						if (!FrameHandler.load(glcontext.getGL().getGL2(), files[i]))
						{
							System.out.println("..load (" + files[i] + ") failed");
						}
					}
				}
				else
				{
					System.out.println("..load file failed, basename null");
				}								
			}			
		});
			
		// save current file
		MenuItem saveFile = new MenuItem(popupMenu, SWT.NONE);
		saveFile.setText("&Save");
		saveFile.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				System.out.println("save file..");
			}
		});
		
		// histogram
		MenuItem histo = new MenuItem(popupMenu, SWT.NONE);
		histo.setText("&Histogram");
		histo.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				
				Image i = new Image(FrameHandler.currFilename);
				i.Load();
				Histogram h = new Histogram(i, 0);
				int [] bins = h.Bins();
				GLWindow GL_histo = new GLHistogram(glcanvas.getDisplay(), "histogram", 450, 180, bins);
				
			}
		});
		
		// quit application
		MenuItem exit = new MenuItem(popupMenu, SWT.NONE);
		exit.setText("&Exit");
		exit.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				System.out.println("exit..");
			}
		});

		glcanvas.setMenu(popupMenu);	
	}
}

class GLHistogram extends GLWindow
{
	int[] hBins = null;
	int max = 0;
	int min = 255;
	public GLHistogram(Display d, String s, int w, int h) { super(d,s,w,h); }
	public GLHistogram(Display d, String s, int w, int h, int[] histoBins) { super(d,s,w,h); hBins = histoBins; setMax();}
	
	void setMax()
	{
    	for (int j=0; j<hBins.length; ++j)
    	{
    		if (hBins[j] > max) max = hBins[j];
    		if (hBins[j] < min) min = hBins[j];
    	}
	}
	
	public void paint(GL2 gl2, int w, int h)
	{
		gl2.glClearColor(0f, 0f, 0f, 0f);
		gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
		gl2.glLoadIdentity();
		if (hBins != null)
		{
			gl2.glEnable(GL2.GL_LINE_SMOOTH);
			gl2.glHint(GL2.GL_LINE_SMOOTH_HINT,  GL2.GL_NICEST);
			gl2.glClearColor(1f, 1f, 1f, 1f);

			// background gradient
			gl2.glBegin(gl2.GL_QUADS);
			gl2.glColor3f( .85f, 0.8f, 0.8f );
			gl2.glVertex3f( 0f, 0f, 0f );
			gl2.glVertex3f( 0f, h, 0f );
			gl2.glColor3f( 0.15f, 0.1f, 0.1f );
			gl2.glVertex3f( w, h, 0f );
			gl2.glVertex3f( w, 0, 0f );
			gl2.glEnd(); 

        	//gl2.glPushMatrix();
        	//gl2.glScalef(w, h, 1);
        	//gl2.glPopMatrix();
			
			// draw histo bins
			float dX = (float) w / (float) hBins.length;
			if (dX >= (float)400 / (float)hBins.length) dX = (float) 400 / (float) hBins.length;
			gl2.glBegin(GL2.GL_LINES);
        	for (int j=0; j<hBins.length; ++j)
        	{
        		float yVal = (float) (h * hBins[j]) / (float) max;
        		gl2.glColor3f((float) j / (float) hBins.length, 0f, 0f);
        		gl2.glVertex2d((float)((j)*dX), h-yVal);	
        	}        	
        	gl2.glEnd();
        	

		}
	}
}