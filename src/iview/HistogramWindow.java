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
	Vec2 boundsr = null;
	Vec2 boundsg = null;
	Vec2 boundsb = null;
	public final Vec2 MousePos = new Vec2(0f, 0f);
	public final Vec2 MouseDelta = new Vec2(0f,0f);
	private float center = 0;
	private float dist = 0;
	private float prevDist = 0;
	private boolean mouseDown = false;
	private boolean dragCenter = false;
	
	public HistogramWindow(Display parent, String title, int w, int h) 
	{
		super(parent, title, w, h);
		// TODO Auto-generated constructor stub
	}
	

	public HistogramWindow(Display d, String s, int w, int h, Histogram hi) 
	{ 
		super(d,s,w,h); 
		if (hi.Bins() != null) 
		{
			int[] histoBins = hi.Bins();
			hBins = new int[histoBins.length];
			for (int j=0; j < histoBins.length; ++j) hBins[j] = histoBins[j]; 
		}
		boundsr = hi.redBounds();
		boundsg = hi.greenBounds();
		boundsb = hi.blueBounds();
		
		center = (float) w/2f;
		dist = (float) w/2f-1f;
		
		setMax();
	}
	
	void setMax()
	{
		if (hBins == null) return;
    	for (int j=0; j<hBins.length; ++j)
    	{
    		if (hBins[j] > max) max = hBins[j];
    		if (hBins[j] < min) min = hBins[j];
    	}
    	
	}
	@Override
	public void onResize(GL2 gl2, int w, int h) 
	{
		if (w < 200) w = 200;
		if (h < 200) h = 200;
		gl2.glViewport( 0, 0, w, h);
        gl2.glMatrixMode( GL2.GL_PROJECTION );
        gl2.glClearColor(0f, 0f, 0f, 0f);
        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
        gl2.glLoadIdentity();               
        gl2.glOrtho(0, w, 0, h, -1, 1);        
        gl2.glMatrixMode(GL2.GL_MODELVIEW);       
        refresh();
	}
	
	@Override
	public void paint(GL2 gl2, int w, int h)
	{
		if (hBins == null) return;
		gl2.glViewport(0, 0, w, h);
		gl2.glMatrixMode(GL2.GL_PROJECTION);
		gl2.glLoadIdentity();
		gl2.glOrtho(0, w, 0, h, -1, 1);
		gl2.glMatrixMode(GL2.GL_MODELVIEW);
//        gl2.glMatrixMode( GL2.GL_PROJECTION );
//        gl2.glClearColor(0f, 0f, 0f, 0f);
//        gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT); // helps to stop flickering on resize        
//        gl2.glLoadIdentity();               
//        gl2.glOrtho(0, w, h, 0, 0, 1);
//
//        gl2.glMatrixMode(GL2.GL_MODELVIEW);
//        gl2.glViewport( 0, 0, w, h);
//
//		gl2.glClearColor(0f, 0f, 0f, 0f);
//		gl2.glClear(GL.GL_DEPTH_BUFFER_BIT | GL.GL_COLOR_BUFFER_BIT);
//		gl2.glLoadIdentity();		
	
		gl2.glEnable(GL2.GL_LINE_SMOOTH);
		gl2.glHint(GL2.GL_LINE_SMOOTH_HINT,  GL2.GL_NICEST);

		// background gradient
		gl2.glPushMatrix();
		gl2.glBegin(GL2.GL_QUADS);
		gl2.glColor3f(.85f, 0.8f, 0.8f);
		gl2.glVertex3f(0f, 0f, 0f);
		gl2.glVertex3f(0f, h, 0f);
		gl2.glColor3f(0.15f, 0.1f, 0.1f);
		gl2.glVertex3f(w, h, 0f);
		gl2.glVertex3f(w, 0, 0f);
		gl2.glEnd();
		gl2.glPopMatrix();

	 

		// draw histo bins
//		float dX = (float) w / (float) hBins.length;
//		if (dX >= (float) 400 / (float) hBins.length)
//			dX = (float) 400 / (float) hBins.length;

		// bins space - map bins-length = width
		gl2.glPushMatrix();
		gl2.glScalef((float)w/(float)hBins.length, 1, 1);
		//gl2.glColor3f(0.85f, 0f, 0f);
		//gl2.glEnable(GL2.GL_BLEND);
		//gl2.glBlendFunc(GL2.GL_ONE, GL2.GL_ONE);
		gl2.glBegin(GL2.GL_LINES);
		for (int j = 0; j < hBins.length; ++j)
		{
			float yVal = (float) (h * hBins[j]) / (float) max;
			gl2.glColor3f((float) j / (float) (hBins.length+1), 0f, 0f);
			gl2.glVertex2f(j, 0);
			gl2.glVertex2f(j, yVal);
			
		}
		//gl2.glDisable(GL2.GL_BLEND);
		
		gl2.glEnd();
		gl2.glPopMatrix();
		
		updateDragLines(gl2, w, h);		

		gl2.glPushMatrix();
		gl2.glScalef((float)w/(float)hBins.length, 1, 1);
		gl2.glBegin(GL2.GL_LINES);
		double lineIdx = (float) ((float) hBins.length / (float) w * MousePos.x);

		gl2.glColor3f(0f, 1f, 0f);
		for (int j = (int) lineIdx - 8; j < (int) lineIdx + 8; ++j)
		{
			if (j < 0 || j > hBins.length - 1) continue;
			float yVal = (float) (h * hBins[j]) / (float) max;
			gl2.glVertex2d((float) ((j) ), 0);
			gl2.glVertex2d((float) ((j + 1) ), yVal);
		}
		gl2.glEnd();
		gl2.glPopMatrix();
		
	}

	private void updateDragLines(GL2 gl2, float w, float h)
	{
		// middle (draggable) vertical bar
		gl2.glPushMatrix();
		gl2.glBegin(GL2.GL_LINES);
		gl2.glLineWidth(300);
		gl2.glColor3f(0.6f, 0.6f, 0.6f);
		for (int j = -2; j < 2; ++j)
		{
			gl2.glVertex2f(center+j, h);
			gl2.glVertex2f(center+j, 0);
		}

		gl2.glEnd();
		gl2.glPopMatrix();
		
		// left (draggable) vertical bar
		gl2.glPushMatrix();
		gl2.glBegin(GL2.GL_LINES);
		gl2.glLineWidth(300);
		gl2.glColor3f(0.1f, 0.1f, 0.1f);
		gl2.glVertex2f(center - dist, h);
		gl2.glVertex2f(center - dist, 0);
		gl2.glEnd();
		gl2.glPopMatrix();
		
		// right (draggable) vertical bar
		gl2.glPushMatrix();
		gl2.glBegin(GL2.GL_LINES);
		gl2.glLineWidth(300);
		gl2.glColor3f(1f, 1f, 1f);
		gl2.glVertex2f(center+dist, h);
		gl2.glVertex2f(center+dist, 0);
		gl2.glEnd();
		gl2.glPopMatrix();
		
		if (prevDist != dist) 
		{
			prevDist = dist;
			updateContrast(w);
		}
	}
	
	private void updateContrast(float w)
	{
		float minIdx = (float) ((float) hBins.length / (float) w * (center-dist));
		float maxIdx = (float) ((float) hBins.length / (float) w * (center+dist));
		if ((int)minIdx < 0) minIdx = 0;
		if ((int)minIdx > hBins.length) minIdx = hBins.length-2;
		if ((int)maxIdx > hBins.length) maxIdx = hBins.length-1;
		if ((int)maxIdx < 1) maxIdx = 1;
		//System.out.println(minIdx + " " + maxIdx);
		Iview.UpdateFromHisto((int)minIdx, (int)maxIdx, (float) boundsr.x, (float) boundsr.y, 
				(float) boundsg.x, (float) boundsg.y, (float) boundsb.x, (float) boundsb.y);
	}
	
	@Override
	public void onMouseScroll(MouseEvent e) 
	{
		prevDist = dist;
		//if (mouseDown && dragCenter)
		{
			if (e.count > 0) dist *= 1.05;
			else if (e.count < 0) dist /= 1.05;
		}	
		refresh();
	}

	@Override
	public void onMouseMove(MouseEvent e) 
	{
		MouseDelta.x += (e.x - MousePos.x);
		MouseDelta.y += (e.y - MousePos.y);
		MousePos.x = e.x;
		MousePos.y = e.y;
		
		if (mouseDown && dragCenter)
		{
			center = (float) MousePos.x;
		}
		
		refresh();	
	}

	@Override
	public void onMouseDown(Event e) 
	{
		mouseDown = true;
		MouseDelta.x = 0;
		MouseDelta.y = 0;
		if (Math.abs(MousePos.x - center) <= 10)
		{
			dragCenter = true;
		}
	}

	@Override
	public void onMouseUp(Event e) 
	{
		if (mouseDown)
		{
			mouseDown = false;
		}
	}

	@Override
	public void onMouseDoubleClick(Event e) 
	{
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onClose() 
	{
		hBins = null;
	}

}
