package iview;
import java.nio.ByteBuffer;

public class Histogram
{	
	enum ColorType { RED, GREEN, BLUE };
	
	ColorType ctype;
	int bins[] = null;
	Boolean normalized = false;
	ByteBuffer data;
	int max = 0;   // assumes char data :(
	int min = 255; // assumes char data :(
	int stride = 0;
	int offset = 0;
	int width = 0;
	Vec2 boundsr = null;
	Vec2 boundsg = null;
	Vec2 boundsb = null;
	final int nBins = 4096;
	public Histogram(Image i, int color)
	{
		if (!setOffset(color, i.Width()))
		{
			System.out.println("..passed invalid color type to histogram, using default (RED)");
			ctype = ColorType.RED;
			offset = 0;
		}
		stride = i.Width() * i.Components();
		width = i.Width();
		data = ByteBuffer.allocate(i.Size());
		for (int j=0; j<i.Size(); ++j) data.put(i.pixelData.get(j));
		boundsr = new Vec2(255,-1);
		boundsg = new Vec2(255,-1);
		boundsb = new Vec2(255,-1);
	
		bins = new int[nBins];
		
		//System.out.println("..call setBounds()");
		
		// run setBounds async
		Thread t = new Thread( new Runnable()
		{
			public void run()
			{
				setBounds();
				boundsr = setBounds(0);
				boundsg = setBounds(1);
				boundsb = setBounds(2);
				updateBins();
				//System.out.println("..updateBins() finished");
			}
		});
		t.run();

	}
	
	public Vec2 redBounds() { return boundsr; }
	public Vec2 greenBounds() { return boundsg; }
	public Vec2 blueBounds() { return boundsb; }
	
	Boolean setOffset(int color, int w)
	{
		if (color == 0)
		{
			ctype = ColorType.RED;
			offset = 0;
			return true;
		}
		else if (color == 1)
		{
			ctype = ColorType.GREEN;
			offset = w;
			return true;
		}
		else if (color == 2)
		{
			ctype = ColorType.BLUE;
			offset = w*2;
			return true;
		}
		return false;
	}

	int Unsigned(byte b)
	{
		return (int) b & 0XFF;
	}

	void setBounds()
	{
		for (int j = offset; j < data.capacity(); j+=stride)
		{
			for (int i=j; i< j+width; ++i)
			{
				int val = Unsigned(data.get(i));
				if (val < min) min = val;
				else if (val > max) max = val;
			}
		}
	}
	
	Vec2 setBounds(int o)
	{
		Vec2 bound = new Vec2(256, -1);
		for (int j = o; j < data.capacity(); j+=stride)
		{
			for (int i=j; i< j+width; ++i)
			{
				int val = Unsigned(data.get(i));
				if (val < bound.x) bound.x = val;
				else if (val > bound.y) bound.y = val;
			}
		}
		return bound;
	}
	
	void zeroBins()
	{
		for(int j=0; j<nBins; ++j) bins[j] = 0;
	}
	
	int[] Bins() { return bins; }
	
	void updateBins()
	{
		zeroBins();
	
		//float hscale =  (float) (max - min )/ (float) nBins;
		for (int j = offset; j < data.capacity(); j+=stride)
		{
			for (int i=j; i< j+width; ++i)
			{
				float val = (float) (Unsigned(data.get(i)) - min) / ((float) max - (float) min); 
				++bins[ (int) ( val * (nBins-1)) ];
			}
		}
		
	}
	
}
