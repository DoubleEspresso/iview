package iview;
import com.sun.jna.Pointer;

public class JPEG
{
	Pointer handle;

	public JPEG()
	{
		handle = Libjpeg.instance.jpeg_instance();
	}

	public Boolean load(String fname)
	{
		return Libjpeg.instance.jpeg_load(fname, handle);
	}

	public Boolean save(String fname, int q)
	{
		return Libjpeg.instance.jpeg_save(fname, q, handle);
	}
	
	public int width()
	{
		return Libjpeg.instance.jpeg_width(handle);
	}
	
	public int height()
	{
		return Libjpeg.instance.jpeg_height(handle);
	}
	
	public int components()
	{
		return Libjpeg.instance.jpeg_comps(handle);
	}
	
	public int size()
	{
		return Libjpeg.instance.jpeg_size(handle);
	}
	
	public Pointer data()
	{
		return Libjpeg.instance.jpeg_data(handle);
	}
	
	public Boolean rotate90()
	{
		return Libjpeg.instance.jpeg_rotate(handle);
	}
	
	public Boolean flipv()
	{
		return Libjpeg.instance.jpeg_flipv(handle);
	}
	
	public Boolean fliph()
	{
		return Libjpeg.instance.jpeg_fliph(handle);
	}
}
