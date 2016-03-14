import java.nio.ByteBuffer;


public class Image
{
	ImagePlugin Plugin = null;
	ByteBuffer pixelData;
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
		filename = fname;
		Plugin = new ImagePlugin(filename);
		if (!Plugin.Initialized()) return;
		pluginInstance = Plugin.getInstance();
		pluginType = Plugin.getType();
		initialized = true;
	}
	
	Boolean Initialized() { return initialized; }
	
	public Boolean Load()
	{
		if (!Initialized())
		{
			System.out.println("..image load failed, not initialized") ;
			return false;
		}
		try
		{
			if (!InitJPG())
			{
				System.out.println("..image initJPEG() failed.") ;
				return false;
			}
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
	
	Boolean InitJPG()
	{
		if (pluginInstance == null) 
		{
			System.out.println("..plugin instance null in InitJPG()");
			return false;
		}
		try
		{
			JPEG jimage = ((JPEG) pluginInstance);

			if (!jimage.load(filename)) return false;

			pixelData = jimage.data().getByteBuffer(0, jimage.size());
			
			pixelData.position(0);
			AspecRatio = (float) jimage.width() / (float) jimage.height();
			width = jimage.width();
			height = jimage.height();
			components = jimage.components();
		}
		catch (Exception any)
		{
			System.out.println("..jpeg init exception " + any.getMessage());
			return false;
		}
		return true;
	}
}
