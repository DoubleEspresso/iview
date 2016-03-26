package iview;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.opengl.GLCanvas;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.Shell;
import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.GLContext;


// TODO: 
// 2. investigate memory leaks when loading images (each load creates new JPEG() .. never free pointer buffer to image data in cpp??)
// 2. update title of main window to loaded image location + image info display somewhere
// 2. double-click to re-zoom image
// 3. proper aspect ratios for loaded images + zoom support on mousewheel
// 4. add basic toolbar menu with file filter imops help etc.
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
	protected static ImagePane imgPane;
	
	public static void main(String[] args)
	{
		initializeWindow();
		attachRightClickMenu(imgPane.glcontext, imgPane.glcanvas);
		
		// TODO: drag & drop event
		while (!imgPane.display.isDisposed())
		{
			//if (!imgPane.display.isDisposed()) imgPane.glcanvas.redraw(); 
			if (!imgPane.display.readAndDispatch()) imgPane.display.sleep();
			
		}
		imgPane.glcanvas.dispose();
		imgPane.display.dispose();		
	}

	private static GL2 getGL()
	{
		return imgPane.getGL2instance();
	}
	
	private static void initializeWindow()
	{
        imgPane = new ImagePane(new Display(), "iview", 640, 480);
	}
	
	public static void UpdateGamma(float gr, float gg, float gb)
	{
		System.out.println("..update gamma");
		imgPane.texture.gammaCorrection(gr, gg, gb, 255f, 1f, 0f);
		imgPane.refresh();
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
						imgPane.texture = new GLTexture(getGL(), files[i]);
						imgPane.hasImage = imgPane.texture.Initialized();
						if (imgPane.hasImage)
						{
							imgPane.texture.Gen();
							imgPane.texture.Bind();
							imgPane.texture.TextureImage2D();
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
				if (!imgPane.hasImage) return;

				Histogram h = new Histogram(imgPane.texture.image, 0);
				new GLHistogram(imgPane.display, "histogram", 450, 180, h.Bins());
			}
		});
		
		// gamma correction
		MenuItem gamma = new MenuItem(popupMenu, SWT.NONE);
		gamma.setText("&Gamma Correction");
		gamma.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				if (!imgPane.hasImage) return;	
				//UpdateGamma(0.1f, 0.1f, 0.1f);
				new GammaCorrection(imgPane.display, "gamma correction", 255, 255);			
			}
		});
		
		// sharpen filter
		MenuItem sharpen_filter = new MenuItem(popupMenu, SWT.NONE);
		sharpen_filter.setText("&Sharpen");
		sharpen_filter.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				if (!imgPane.hasImage) return;
				imgPane.texture.Sharpen(3);
				imgPane.refresh();
			}
		});
		
		// sobel filter
		MenuItem sobel_filter = new MenuItem(popupMenu, SWT.NONE);
		sobel_filter.setText("&Sobel");
		sobel_filter.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				if (!imgPane.hasImage) return;
				imgPane.texture.Sobel();
				imgPane.refresh();
			}
		});
		
		// emboss filter
		MenuItem emboss_filter = new MenuItem(popupMenu, SWT.NONE);
		emboss_filter.setText("&Emboss");
		emboss_filter.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				if (!imgPane.hasImage) return;
				imgPane.texture.Emboss(3);
				imgPane.refresh();
			}
		});
		
		// median filter
		MenuItem median_filter = new MenuItem(popupMenu, SWT.NONE);
		median_filter.setText("&Median");
		median_filter.addListener(SWT.Selection, new Listener()
		{
			public void handleEvent(Event e)
			{
				if (!imgPane.hasImage) return;
				imgPane.texture.Median(3);
				imgPane.refresh();
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
	
	public static ImagePane getPane() { return imgPane; }
}



class ImagePane extends GLWindow
{
	public Boolean hasImage = false;
	public GLTexture texture;
	public float scale = 1f;
	public final Vec2 imageSize = new Vec2(1f,1f);
	public final Vec2 imageCorner = new Vec2(0f,0f);
	public float pscale = 1f;
	public double percentX = 1;
	public double percentY = 1;
	public Boolean mouseDown = false;
	public final Vec2 MousePos = new Vec2(0f, 0f);
	public final Vec2 MouseDelta = new Vec2(0f,0f);
	public final Vec2 Translate = new Vec2(0f,0f);
	public final Vec2 ScalePos = new Vec2 (0f, 0f);
	
	public ImagePane(Display d, String s, int w, int h) 
	{ 
		super(d,s,w,h);
	}
	
	private void display(GL2 gl2, float w, float h, float iw, float ih)
	{
		if (scale <= 0) return;
		
		//float dX = 0;//(w - iw)/2f;
		//float dY = 0;//(h - ih)/2f;

		if (scale != 1f)
		{
			gl2.glTranslatef((float) ( ScalePos.x)  , (float) ( ScalePos.y), 0f);
			gl2.glScalef(scale , scale , 0f);
			gl2.glTranslatef(-(float) ( percentX * iw + Translate.x)  , -(float) ( percentY * ih + Translate.y), 0f);	
			//System.out.println("ix(" + percentX * w + ") iy(" + percentY * h + ") " + "mx(" + getMousePos().x + ") my(" + getMousePos().y + ")");
		} 
		
		if (mouseDown)
		{
			gl2.glTranslatef((float)(Translate.x + MouseDelta.x), (float)(Translate.y + MouseDelta.y), 0f);
		}
		else gl2.glTranslatef((float) ( Translate.x)  , (float) ( Translate.y), 0f);
	}
	
//	public void gammaCorrect(float gr, float gg, float gb, float max, float scale, float bias)
//	{
//		texture.gammaCorrection(gr, gg, gb, max, scale, bias);
//		refresh();
//	}
	
	public void paint(GL2 gl2, int width, int height)
	{
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        //gl2.glClearColor(0f, 0f, 0f, 0f);
        //gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();               
        gl2.glOrtho(0, width, height, 0, 0, 1);

        gl2.glMatrixMode(GL2.GL_MODELVIEW);
        gl2.glViewport( 0, 0, width, height);
		
		
		gl2.glClearColor(0f, 0f, 0f, 0f);
		gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
		gl2.glLoadIdentity();
		
		
        if (hasImage) 
        {
        	// handle aspect ratio based on window size and image size
        	float iw = (float) texture.image.width;
        	float ih = (float) texture.image.height;       	
        	float s = (float)Math.min(1, (float) width / iw);
        	float t = (float)Math.min(1, (float) height / ih);

			float w = s * iw;
			float h = t * ih;
			if (s != t)
			{
				if (t < s)
					w = t * ih * texture.image.AspecRatio;
				else
					h = s * iw / (texture.image.AspecRatio);
			}

        	imageSize.x = w; imageSize.y = h;
        	// apply scale factors	
        	
        	display(gl2, width, height, w, h);
        	        	
        	gl2.glBindTexture(GL2.GL_TEXTURE_2D, texture.ID());       	
        	gl2.glBegin(GL2.GL_QUADS);
        	gl2.glTexCoord2f(0, 0); gl2.glVertex2d(0, 0);
        	gl2.glTexCoord2f(1, 0); gl2.glVertex2d(w, 0);
        	gl2.glTexCoord2f(1, 1); gl2.glVertex2d(w, h);
        	gl2.glTexCoord2f(0, 1); gl2.glVertex2d(0, h);
        	gl2.glEnd();
        }
        
	}
	
	public void onKeyPressed(KeyEvent e)
	{
		if (!hasImage) return;
		
		pscale = scale;
		ScalePos.x = MousePos.x;
		ScalePos.y = MousePos.y;

		if (e.character == '=')
		{	
			scale *= 1.2f;
			refresh();
		}
		else if (e.character == '-')
		{	
			scale /= 1.2f;
			refresh();
		}
		else if (e.character == 'r')
		{
			texture.Rotate();
			imageSize.set(imageSize.y, imageSize.x);
			refresh();
		}
		else if (e.character == 'v')
		{
			texture.FlipV();
			refresh();
		}
		else if (e.character == 'h')
		{
			texture.FlipH();
			refresh();
		}

		updateDisplayScale();
	}

	private int reqCount = 0;
	public void onMouseScroll(MouseEvent e) 
	{
		int count = e.count;

		if (!hasImage || Math.abs(count) != 3 ) return;
		
		// workaround flickering if mouse scroll events are 
		// issued "too quickly" (seems to be swt version dependent)
		if (++reqCount % 4 != 0) return;
		
		reqCount = 0;
		pscale = scale;
		ScalePos.x = MousePos.x;
		ScalePos.y = MousePos.y;

		if (count < 0)
		{	
			scale *= 1.3f;
		}
		else if (count > 0)
		{			
			scale /= 1.3f;
		}
		updateDisplayScale();
	}	
	
	public void onMouseDown(Event e) 
	{		
		mouseDown = true;
		MouseDelta.x=0;
		MouseDelta.y=0;
	}
	
	public void onMouseUp(Event e) 
	{ 
		if (mouseDown)
		{
			mouseDown = false;

			Translate.x += MouseDelta.x;
			Translate.y += MouseDelta.y;

			imageCorner.x += MouseDelta.x*scale;
			imageCorner.y += MouseDelta.y*scale;
			
			ScalePos.x = MousePos.x;
			ScalePos.y = MousePos.y;

			MouseDelta.x = 0;
			MouseDelta.y = 0;
			
			
			float ppscale = pscale;
			pscale = scale;
			updateDisplayScale();
			pscale = ppscale;
		}
	}
	public void onMouseDoubleClick(Event e) { }
	
	public void onMouseMove(MouseEvent e)
	{
		MouseDelta.x += (e.x - MousePos.x)/scale;
		MouseDelta.y += (e.y - MousePos.y)/scale;
		MousePos.x = e.x;
		MousePos.y = e.y;
		if (mouseDown) refresh();
		//System.out.println("mouse("+MousePos.x + "," + MousePos.y + ")");
	}
	
	void updateDisplayScale()
	{
		// imageSize is set to starting width/height each time we call updateScale
		double prevImageSizeX = imageSize.x * pscale;
		double prevImageSizeY = imageSize.y * pscale;

		// assume the mouse is "on the image" 
		percentX = (float) (ScalePos.x - imageCorner.x) / (float) prevImageSizeX;
		percentY = (float) (ScalePos.y - imageCorner.y) / (float) prevImageSizeY;
		
		imageSize.x *= scale;
		imageSize.y *= scale;
							
		imageCorner.x += (prevImageSizeX - imageSize.x) * percentX ;
		imageCorner.y += (prevImageSizeY - imageSize.y) * percentY ;
		//System.out.println("mouse("+ScalePos.x + "," + ScalePos.y + "), corner(" + imageCorner.x + "," + imageCorner.y+ ") [" + percentX + "," + percentY + "] imsz(" + imageSize.x + "," + imageSize.y + ")");		
	}		
}


class GLHistogram extends GLWindow
{
	int[] hBins = null;
	int max = 0;
	int min = 255;
	public GLHistogram(Display d, String s, int w, int h) { super(d,s,w,h); }
	public GLHistogram(Display d, String s, int w, int h, int[] histoBins) { super(d,s,w,h); hBins = histoBins; setMax();}
	public final Vec2 MousePos = new Vec2(0f, 0f);
	public final Vec2 MouseDelta = new Vec2(0f,0f);
	
	void setMax()
	{
    	for (int j=0; j<hBins.length; ++j)
    	{
    		if (hBins[j] > max) max = hBins[j];
    		if (hBins[j] < min) min = hBins[j];
    	}
	}
	
	public void onMouseScroll(MouseEvent e) {}
	
	public void paint(GL2 gl2, int w, int h)
	{
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        //gl2.glClearColor(0f, 0f, 0f, 0f);
        //gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();               
        gl2.glOrtho(0, w, h, 0, 0, 1);

        gl2.glMatrixMode(GL2.GL_MODELVIEW);
        gl2.glViewport( 0, 0, w, h);

		gl2.glClearColor(0f, 0f, 0f, 0f);
		gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
		gl2.glLoadIdentity();		
		
		if (hBins != null)
		{
			gl2.glEnable(GL2.GL_LINE_SMOOTH);
			gl2.glHint(GL2.GL_LINE_SMOOTH_HINT,  GL2.GL_NICEST);
			//gl2.glClearColor(1f, 1f, 1f, 1f);


			// background gradient
			gl2.glBegin(GL2.GL_QUADS);
			gl2.glColor3f( .85f, 0.8f, 0.8f );
			gl2.glVertex3f( 0f, 0f, 0f );
			gl2.glVertex3f( 0f, h, 0f );
			gl2.glColor3f( 0.15f, 0.1f, 0.1f );
			gl2.glVertex3f( w, h, 0f );
			gl2.glVertex3f( w, 0, 0f );
			gl2.glEnd(); 			
			
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
        	
			double lineIdx = (float) ((float)hBins.length / (float) width  * MousePos.x);		
			gl2.glColor3f(0f, 1f, 0f);
        	for (int j=(int)lineIdx - 8; j<(int)lineIdx + 8; ++j)
        	{
        		if (j < 0 || j > hBins.length-1) continue; 
        		
        		float yVal = (float) (h * hBins[j]) / (float) max;
        		gl2.glVertex2d((float)((j)*dX), h) ;
        		gl2.glVertex2d((float)((j+1)*dX), h-yVal);        	
        	}
        	gl2.glEnd();
  		}
	}
	
	public void onMouseMove(MouseEvent e)
	{
		MouseDelta.x += (e.x - MousePos.x);
		MouseDelta.y += (e.y - MousePos.y);
		MousePos.x = e.x;
		MousePos.y = e.y;
		
		refresh();
		//System.out.println("mouse("+MousePos.x + "," + MousePos.y + ")");
	}
	public void onMouseDown(Event e) { }
	public void onMouseUp(Event e) { }
	public void onMouseDoubleClick(Event e) { }
}