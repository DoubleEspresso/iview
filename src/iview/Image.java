package iview;
import java.nio.ByteBuffer;
import com.sun.jna.Pointer;

public class Image
{
	Pointer image_handle;
//	ImagePlugin Plugin = null;
	ByteBuffer pixelData;
	ByteBuffer colorData;
	Boolean initialized = false;
	String filename = null;
	Object pluginInstance = null;
	ImagePlugin.PluginType pluginType;
	float AspecRatio = 0;
	int width = 0;
	int height = 0;
	int components = 0;
	
	public Image(String fname)
	{
		image_handle = Libimage.instance.image_instance();
		filename = fname;
//		Plugin = new ImagePlugin(filename);
//		if (!Plugin.Initialized()) return;
//		pluginInstance = Plugin.getInstance();
//		pluginType = Plugin.getType();
		initialized = true;
	}
	
	Boolean Initialized() { return initialized; }
	
	public Boolean Rotate90() 
	{
		try 
		{
			Boolean ok = Libimage.instance.rotate90(image_handle);
			width = Libimage.instance.width(image_handle);
			height = Libimage.instance.height(image_handle);			
			updatePixelData();
			return ok; 
		} 
		catch (Exception any) {
			System.out.println("..exception calling rotate " + any.getMessage());
			return false;
		}
	}
	
	public Boolean FlipV() 
	{
		try 
		{
			Boolean ok = Libimage.instance.flipv(image_handle);
			updatePixelData();
			return ok; 
		} 
		catch (Exception any) {
			System.out.println("..exception calling rotate " + any.getMessage());
			return false;
		}
	}
	
	public Boolean FlipH() 
	{
		try 
		{
			Boolean ok = Libimage.instance.fliph(image_handle);
			updatePixelData();
			return ok; 
		} 
		catch (Exception any) {
			System.out.println("..exception calling rotate " + any.getMessage());
			return false;
		}
	}

	public Boolean Binning(int b) 
	{
		try 
		{
			Boolean ok = Libimage.instance.binning(b, image_handle);
			updatePixelData();
			return ok; 
		} 
		catch (Exception any) {
			System.out.println("..exception calling rotate " + any.getMessage());
			return false;
		}
	}
	
	public Boolean MeanFilter(int r) 
	{
		try 
		{
			Boolean ok = Libimage.instance.mean_filter(r, image_handle);
			updatePixelData();
			return ok; 
		} 
		catch (Exception any) {
			System.out.println("..exception calling mean filter " + any.getMessage());
			return false;
		}
	}
	
	public Boolean Load()
	{
		if (!Initialized())
		{
			System.out.println("..image load failed, not initialized") ;
			return false;
		}
		try
		{

			if (image_handle == null)
			{
				System.out.println("..image instance null in Load()");
				return false;
			}

			if (!Libimage.instance.load(filename, image_handle))
			{
				System.out.println("..image lib load failed!");
				return false;
			}

			updatePixelData();

			// TODO: fix this switch statement (not working now).
//			switch (pluginType)
//			{
//				case JPEG : 
//					if (!InitJPG())
//					{
//						System.out.println("..image initJPEG() failed.") ;
//						return false;
//					}
//					else System.out.println("..DBG InitJPG() OK") ;
//					break;
//				case TIFF : return false;
//				case PNG  : return false;
//				case GIF  : return false;
//				case JP2  : return false;
//				case FPX  : return false;
//				case PCD  : return false;
//			}
//			
			return true;
		}
		catch (Exception any)
		{
			System.out.println("..image load exception " + any.getMessage()) ;
			return false;
		}
	}
	
	int Width() { return width; }
	int Height() { return height; }
	ByteBuffer PixelData() { return pixelData; }
	int Components() { return components; }
	int Size() { return width * height * components; }
	
	Boolean updatePixelData()
	{
		int data_size = Libimage.instance.size(image_handle)
				* Libimage.instance.comps(image_handle);
		byte[] gldata = new byte[data_size]; // 1 char = 4 bytes
		if (!Libimage.instance
				.texture_data(gldata, data_size, image_handle))
		{
			System.out.println("..failed to get image texture data!");
			return false;
		}

		pixelData = ByteBuffer.wrap(gldata);											
		pixelData.position(0);
		AspecRatio = (float) Libimage.instance.width(image_handle) / (float) Libimage.instance.height(image_handle);
		width = Libimage.instance.width(image_handle);
		height = Libimage.instance.height(image_handle);
		components = Libimage.instance.comps(image_handle);
		return true;
	}
	
	public void sharpen(int size)
	{
		Libimage.instance.sharpen(size, image_handle);
		width = Libimage.instance.width(image_handle);
		height = Libimage.instance.height(image_handle);			
		updatePixelData();
	}
	public void sobel()
	{
		Libimage.instance.sobel(image_handle);
		width = Libimage.instance.width(image_handle);
		height = Libimage.instance.height(image_handle);			
		updatePixelData();

	}
	public void emboss(int ksize)
	{
		Libimage.instance.emboss(ksize, image_handle);
		width = Libimage.instance.width(image_handle);
		height = Libimage.instance.height(image_handle);			
		updatePixelData(); 
	}
	
	public void gaussianSmooth(int ksize, float s)
	{
		Libimage.instance.gaussian_smooth(ksize, s, image_handle);
		updatePixelData();
	}
	
	public void median(int r)
	{
		Libimage.instance.median(r, image_handle);
		width = Libimage.instance.width(image_handle);
		height = Libimage.instance.height(image_handle);			
		updatePixelData(); 
	}
	
	public Boolean gammaCorrection(float gr, float gg, float gb, float max, float scale, float bias)
	{
		Boolean ok = Libimage.instance.gamma_correct(gr, gg, gb, max, scale, bias, image_handle);
		updatePixelData();
		return ok;
	}
	 
	public Boolean grayScale()
	{
		Boolean ok = Libimage.instance.gray_scale(image_handle);
		updatePixelData();
		return ok;
	}
	
	public Boolean threshold(float val)
	{
		Boolean ok = Libimage.instance.threshold(val, image_handle);
		updatePixelData();
		return ok;
	}
	
	public Boolean NonlocalMeans(int r, int sz)
	{
		Boolean ok = Libimage.instance.nonlocal_means(r, sz, image_handle);
		updatePixelData();
		return ok;
	}
	
	public Boolean save(String fname)
	{
		Libimage.instance.save(fname, 100, image_handle);
		return true;
	}
}
