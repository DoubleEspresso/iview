
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.KeyListener;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.MouseWheelListener;
import org.eclipse.swt.events.PaintEvent;
import org.eclipse.swt.events.PaintListener;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.opengl.GLCanvas;
import org.eclipse.swt.opengl.GLData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;
import com.jogamp.opengl.GLContext;
import com.jogamp.opengl.GLDrawableFactory;
import com.jogamp.opengl.GLProfile;

public abstract class GLWindow
{

	public Display display = null;
	public Shell shell = null;
	public Composite composite = null;
	public GLCanvas glcanvas = null;
	public GLContext glcontext = null;
	
	private Boolean is_initialized = false;
	public int width = 0;
	public int height = 0;
	public static int minWidth = 10;
	public static int minHeight = 10;
	
	public GLWindow(Display parent, String title, int w, int h)
	{
		display = parent;
        shell = new Shell( display );
        shell.setText( title );
        shell.setLayout( new FillLayout() );
        width = w;
        height = h;
        shell.setSize( width, height );

        composite = new Composite( shell, SWT.NONE );
        composite.setLayout( new FillLayout() );
        
        GLData gldata = new GLData();
        gldata.doubleBuffer = true;
        
        // need SWT.NO_BACKGROUND to prevent SWT from clearing the window at the wrong times (we use glClear for this instead)
        glcanvas = new GLCanvas( composite, SWT.NO_BACKGROUND, gldata );
        glcanvas.setCurrent();
        GLProfile glprofile = GLProfile.getDefault();
        
        glcontext = GLDrawableFactory.getFactory( glprofile ).createExternalGLContext();
        initGL(glcontext); 
        
        // add event listeners
        {
        	addResizeListener();
        	addMouseWheelListener();
        	addMouseMoveListener();
        	addMouseDownListener();
        	addMouseUpListener();
        	addDoubleClickListener();
        	addPaintListener();
        	addKeyListener();
        }
        
        is_initialized = true;
        shell.open();
	}		

	public Boolean initialized()
	{
		return is_initialized;
	}
	
	private static void initGL(GLContext glcontext)
	{
		glcontext.getGL().glEnable(GL.GL_TEXTURE_2D);
		glcontext.getGL().glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	}
	
	public GL2 getGL2instance()
	{
		return glcontext.getGL().getGL2();
	}
	
	private void addResizeListener()
	{
		glcanvas.addListener(SWT.Resize, new Listener()
		{
			@Override 
			public void handleEvent(Event e)
			{
				resizeWindow();
			}
		});
	}

	private void addMouseWheelListener()
	{
		glcanvas.addMouseWheelListener(new MouseWheelListener()
		{
			@Override
			public void mouseScrolled(MouseEvent e)
			{
				glcanvas.setCurrent();
				glcontext.makeCurrent();
				onMouseScroll(e);
				glcontext.release();
			}
		});
	}
	
	private void addMouseDownListener()
	{
		glcanvas.addListener(SWT.MouseDown, new Listener()
		{
			@Override 
			public void handleEvent(Event e)
			{
				if (e.button == 1) // left mouse button
				{
					glcanvas.setCurrent();
					glcontext.makeCurrent();
					onMouseDown(e);
					glcontext.release();
				}
			}
		});
	}
	
	private void addMouseUpListener()
	{
		glcanvas.addListener(SWT.MouseUp, new Listener()
		{
			@Override 
			public void handleEvent(Event e)
			{
				if (e.button == 1) // left mouse button
				{
					glcanvas.setCurrent();
					glcontext.makeCurrent();
					onMouseUp(e);
					glcontext.release();
				}
			}
		});
	}
	
	private void addDoubleClickListener()
	{
		glcanvas.addListener(SWT.MouseDoubleClick, new Listener()
		{
			@Override 
			public void handleEvent(Event e)
			{
				onMouseDoubleClick(e);
			}
		});
	}
	
	private void addMouseMoveListener()
	{		
		glcanvas.addMouseMoveListener(new MouseMoveListener()
		{			
			public void mouseMove(MouseEvent e)
			{
				glcanvas.setCurrent();
				glcontext.makeCurrent();
				onMouseMove(e);
				glcontext.release();
			}
		});
	}
	
	private void addPaintListener()
	{
		glcanvas.addPaintListener(new PaintListener()
		{
			@Override
			public void paintControl(PaintEvent paintevent)
			{
				renderGLWindow();
			}
		});
	}

	private void addKeyListener()
	{
		glcanvas.addKeyListener(new KeyListener()
		{
			@Override
			public void keyPressed(KeyEvent e)
			{
				onKeyPressed(e);
			}

			@Override
			public void keyReleased(KeyEvent e)
			{
				onKeyReleased(e);
			}
		});
	}
	
	private void resizeWindow()
	{
		Rectangle r = glcanvas.getClientArea();
		if (r.width < minWidth)
		{
			glcanvas.setSize(minWidth, r.height);
			return;
		}
		else if (r.height < minHeight)
		{
			glcanvas.setSize(r.width, minHeight);
			return;
		}
		glcanvas.setCurrent();
		glcontext.makeCurrent();
		onResize(getGL2instance(), r.width, r.height);
		glcontext.release();
	}

	public void renderGLWindow()
	{

		Display.getDefault().asyncExec(new Runnable()
		{
			public void run()
			{
				if (!glcanvas.isDisposed()) // avoid errors on exit
				{
					Rectangle r = clientSize();
					glcanvas.setCurrent();
					glcontext.makeCurrent();
					paint(getGL2instance(), r.width, r.height);
					glcanvas.swapBuffers();
					glcontext.release();
				}
			}
		});
	}
	
	public Rectangle clientSize()
	{
		return glcanvas.getClientArea();
	}
	
	public void onResize(GL2 gl2, int w, int h) 
	{
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();               
        gl2.glOrtho(0, w, h, 0, 0, 1);        
        gl2.glViewport( 0, 0, w, h);
        gl2.glMatrixMode(GL2.GL_MODELVIEW);
	}
	
	public abstract void paint(GL2 gl2, int w, int h);
	public abstract void onMouseScroll(MouseEvent e);
	public abstract void onMouseMove(MouseEvent e);
	public abstract void onMouseDown(Event e);
	public abstract void onMouseUp(Event e);
	public abstract void onMouseDoubleClick(Event e);
	
	public void onKeyPressed(KeyEvent e) {}
	public void onKeyReleased(KeyEvent e) {}
}
