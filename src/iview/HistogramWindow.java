package iview;

import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;

import com.jogamp.opengl.GL;
import com.jogamp.opengl.GL2;

public class HistogramWindow extends GLWindow
{
	int[] hBins = null;
	int max = 0;
	int min = 255;
	public final Vec2 MousePos = new Vec2(0f, 0f);
	public final Vec2 MouseDelta = new Vec2(0f,0f);
	
	public HistogramWindow(Display parent, String title, int w, int h) 
	{
		super(parent, title, w, h);
		// TODO Auto-generated constructor stub
	}
	

	public HistogramWindow(Display d, String s, int w, int h, int[] histoBins) 
	{ 
		super(d,s,w,h); 
		hBins = histoBins; 
		setMax();
	}
	
	void setMax()
	{
    	for (int j=0; j<hBins.length; ++j)
    	{
    		if (hBins[j] > max) max = hBins[j];
    		if (hBins[j] < min) min = hBins[j];
    	}
	}
	
	@Override
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

	@Override
	public void onMouseScroll(MouseEvent e) 
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onMouseMove(MouseEvent e) 
	{
		MouseDelta.x += (e.x - MousePos.x);
		MouseDelta.y += (e.y - MousePos.y);
		MousePos.x = e.x;
		MousePos.y = e.y;
		
		refresh();	
	}

	@Override
	public void onMouseDown(Event e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onMouseUp(Event e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onMouseDoubleClick(Event e) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onClose() {
		// TODO Auto-generated method stub
		
	}

}
