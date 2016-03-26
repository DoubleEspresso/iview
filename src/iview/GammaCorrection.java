package iview;

import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;

public class GammaCorrection extends GLWindow
{
	public float RedGamma = 1;
	public float BluGamma = 1;
	public float GrnGamma = 1;
	
	public boolean selectedRed = false;
	public boolean selectedGreen = false;
	public boolean selectedBlue = false;
	
	private float RedY[];
	private float GrnY[];
	private float BluY[];
	
	public float max = 255;
	private boolean mouseDown = false;
	public final Vec2 MousePos = new Vec2(0f, 0f);
	public final Vec2 MouseDelta = new Vec2(0f,0f);

	public GammaCorrection(Display d, String s, int w, int h) 
	{ 
		super(d,s,w,h);
		RedY = new float[512];
		GrnY = new float[512];
		BluY = new float[512];
		
		float dx = 0.5f;
		for (int j=0; j<512; ++j)
		{
			RedY[j] = OutputIntensity(dx * j, RedGamma);
			GrnY[j] = OutputIntensity(dx * j, GrnGamma);
			BluY[j] = OutputIntensity(dx * j, BluGamma);
		}


	}
		
	public void onMouseDoubleClick(Event e) { }
	public void onMouseScroll(MouseEvent e) {}
	
	public float OutputIntensity(float I, float g)
	{
		return (float) ((float) max * Math.pow((double)(I / max), (double)g));
	}
	
	public float GetGamma(float Ix, float Iy)
	{
		return (float) (Math.log10(Iy/max) / Math.log10(Ix / max));
	}

	public void paint(GL2 gl2, int w, int h)
	{
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        gl2.glClearColor(0f, 0f, 0f, 0f);
        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();               
        gl2.glOrtho(0, w, h, 0, 0, 1);

        gl2.glMatrixMode(GL2.GL_MODELVIEW);
        gl2.glViewport( 0, 0, w, h);

		gl2.glClearColor(0f, 0f, 0f, 0f);
		gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
		gl2.glLoadIdentity();		
	
		gl2.glEnable(GL2.GL_LINE_SMOOTH);
		gl2.glHint(GL2.GL_LINE_SMOOTH_HINT,  GL2.GL_NICEST);

		// background gradient
		gl2.glBegin(GL2.GL_QUADS);
		gl2.glColor3f( .8f, 0.8f, 0.8f );
		gl2.glVertex3f( 0f, 0f, 0f );
		gl2.glVertex3f( 0f, h, 0f );
		gl2.glColor3f( 0.1f, 0.1f, 0.1f );
		gl2.glVertex3f( w, h, 0f );
		gl2.glVertex3f( w, 0, 0f );
		gl2.glEnd(); 
		
		gl2.glBegin(GL2.GL_LINES);
    	for (int j=0; j<512; ++j)
    	{
    		gl2.glColor3f(1f, 0f, 0f);    
    		gl2.glVertex2d((float)((j)*0.5f), max-RedY[j]);
      	} 
    	gl2.glEnd();
    	
		gl2.glBegin(GL2.GL_LINES);
    	for (int j=0; j<512; ++j)
    	{
    		gl2.glColor3f(0f, 1f, 0f);   
    		gl2.glVertex2d((float)((j)*0.5f), max-GrnY[j]);
      	} 
    	gl2.glEnd();
 		
		gl2.glBegin(GL2.GL_LINES);
    	for (int j=0; j<512; ++j)
    	{    		
    		gl2.glColor3f(0f, 0f, 1f); 
    		gl2.glVertex2d((float)((j)*0.5f), max-BluY[j]);
      	} 
    	gl2.glEnd();

	}
	
	public void onMouseDown(Event e) 
	{		
		mouseDown = true;
		MouseDelta.x=0;
		MouseDelta.y=0;
		selectedRed = false;
		selectedGreen = false;
		selectedBlue = false;
		float gamma = GetGamma((float)MousePos.x, (float)(max - MousePos.y));
		System.out.println("(" + MousePos.x + "," + (max-MousePos.y) +") " + gamma + " rg " + RedGamma + " gg " + GrnGamma + " bg " + BluGamma );
		if (Math.abs(gamma - RedGamma) <= .2) selectedRed = true;
		else if (Math.abs(gamma - GrnGamma) <= .2) selectedGreen = true;
		else if (Math.abs(gamma - BluGamma) <= .2) selectedBlue = true;
	}
	
	public void onMouseUp(Event e) 
	{ 
		if (mouseDown)
		{
			mouseDown = false;
			MouseDelta.x = 0;
			MouseDelta.y = 0;
			
			if (selectedRed) updateRed();
			else if (selectedGreen) updateGreen();
			else if (selectedBlue) updateBlue();						
			selectedRed = false;
			selectedGreen = false;
			selectedBlue = false;
			refresh();
			//Iview.UpdateGamma(RedGamma, GrnGamma, BluGamma);//, 255f, 1f, 0f);
			//Iview.imgPane.refresh();
		}
	}
	public void onMouseMove(MouseEvent e)
	{
		MouseDelta.x += (e.x - MousePos.x);
		MouseDelta.y += (e.y - MousePos.y);
		MousePos.x = e.x;
		MousePos.y = e.y;
		if (mouseDown) 
		{
			if (selectedRed) updateRed();
			else if (selectedGreen) updateGreen();
			else if (selectedBlue) updateBlue();						
			refresh();
		}
	}
	private void updateRed()
	{
		float y = (float)MousePos.y;
		float x = (float)MousePos.x;
		float pGamma = RedGamma;
		RedGamma = GetGamma(x, max-y);
		if (pGamma != RedGamma)
		{
			float dx = 0.5f;
			for (int j=0; j<512; ++j)
			{
				RedY[j] = OutputIntensity(dx * j, RedGamma);
			}			
		}
	}
	private void updateGreen()
	{
		GrnGamma = GetGamma((float)MousePos.x, max - (float)MousePos.y);
		float dx = 0.5f;
		for (int j=0; j<512; ++j)
		{
			GrnY[j] = OutputIntensity(dx * j, GrnGamma);
		}
	}
	private void updateBlue()
	{
		BluGamma = GetGamma((float)MousePos.x, max - (float)MousePos.y);
		float dx = 0.5f;
		for (int j=0; j<512; ++j)
		{
			BluY[j] = OutputIntensity(dx * j, BluGamma);
		}
	}
}
