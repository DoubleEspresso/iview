package iview;

public class ImagePlugin
{
	private Object pluginInstance = null;	
	String extension = null;		
	enum PluginType { JPEG, TIFF, PNG, GIF, JP2, FPX, PCD }
	PluginType plugin = PluginType.JPEG; 
	Boolean initialized;
	String filename = null;
	
//	Boolean hasJpeg = false;
//	Boolean hasTiff = false;
//	Boolean hasPng = false;
//	Boolean hasGif = false;
//	Boolean hasJp2 = false;
//	Boolean hasFpx = false;
//	Boolean hasPcd = false;
	
	public ImagePlugin(String fname)
	{
		try
		{
			extension = StringUtils.extension(fname);
			initialized = init();
			filename = fname;
		}
		catch(Exception any)
		{
			System.out.println("..image plugin failed to initialize " + any.getMessage());
		}
	}
	
	public Boolean Initialized() { return initialized; }
	public Object getInstance() { return pluginInstance ; }
	public PluginType getType() { return plugin; }
	
	Boolean init()
	{
		extension = extension.toLowerCase();
		switch (extension)
		{
			case ".jpg"  : 
				plugin = PluginType.JPEG; 
				pluginInstance = new JPEG();				
			case ".jpeg" : 
				plugin = PluginType.JPEG; pluginInstance = new JPEG();
			case ".tif"  : 
				plugin = PluginType.TIFF; //pluginInstance = new TIF();
			case ".tiff" : 
				plugin = PluginType.TIFF; //pluginInstance = new TIF();
			case ".gif"  : 
				plugin = PluginType.GIF; //pluginInstance = new TIF();
			case ".giff" : 
				plugin = PluginType.GIF; //pluginInstance = new TIF();
			case ".jp2"  : 
				plugin = PluginType.JP2; //pluginInstance = new JP2();
			case ".fpx"  :
				plugin = PluginType.FPX; //pluginInstance = new FPX();
			case ".pcd"  :
				plugin = PluginType.FPX; //pluginInstance = new PCD();				
		}
		if (pluginInstance == null)
		{
			System.out.println("..unsupported extension " + extension);
			return false;
		}
		else
		{
			System.out.println("..plugin instance created for " + extension);
			return true;
		}
	}
	
	public Boolean ImageData()
	{
		try
		{
			if (!initialized) return false;
			boolean loadOK = false;
			switch (plugin)
			{
				case JPEG : loadOK = ((JPEG)pluginInstance).load(filename);
				case TIFF : loadOK = false;
				case PNG  : loadOK = false;
				case GIF  : loadOK = false;
				case JP2  : loadOK = false;
				case FPX  : loadOK = false;
				case PCD  : loadOK = false;
			}
			
			return loadOK;
		}
		catch(Exception any)
		{
			System.out.println(".. plugin load failed " + any.getMessage());
			return false;
		}
	}
}
