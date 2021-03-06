package iview;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;


public interface Libjpeg extends Library
{
	Libjpeg instance = (Libjpeg)Native.loadLibrary("imgJpeg",Libjpeg.class);
	public Pointer jpeg_instance();
	public Boolean jpeg_load(String filename, Pointer handle);
	public Boolean jpeg_save(String fname, int quality, Pointer handle);
	public int jpeg_width(Pointer handle);
	public int jpeg_height(Pointer handle);
	public int jpeg_comps(Pointer handle);
	public Pointer jpeg_data(Pointer handle);
	public int jpeg_size(Pointer handle);
	public Boolean jpeg_rotate(Pointer handle);
	public Boolean jpeg_flipv(Pointer handle);
	public Boolean jpeg_fliph(Pointer handle);
}

