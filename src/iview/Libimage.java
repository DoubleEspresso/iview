package iview;
import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;


public interface Libimage extends Library
{
	Libimage instance = (Libimage)Native.loadLibrary("image",Libimage.class);
	public Pointer image_instance();
	public Boolean load(String filename, Pointer handle);
	public Boolean save(String fname, int quality, Pointer handle);
	public Boolean init(int w, int h, int c, Pointer handle);
	public int comps(Pointer handle);
	public int width(Pointer handle);
	public int height(Pointer handle);
	public int size(Pointer handle);
	public Boolean rotate90(Pointer handle);
	public void sharpen(int size, Pointer handle);
	public void sobel(Pointer handle);
	public void emboss(int ksize, Pointer handle);
	public void gaussian_smooth(int ksize, float sigma, Pointer handle);
	public void median(int r, Pointer handle);
	public Boolean texture_data(byte[] data, int size, Pointer handle);
	public Boolean gamma_correct(float gr, float gg, float gb, float max, float scale, float bias, Pointer handle);
	public Boolean gray_scale(Pointer handle);
	public Boolean threshold(float val, Pointer handle);
}