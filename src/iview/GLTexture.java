package iview;
import com.jogamp.opengl.GL2;

public class GLTexture
{
	private int width;
	private int height;
	private int texture_id;
	Boolean initialized = false;
	Image image = null;
	GL2 GL;
	int[] textureBuffers;
	
	public GLTexture(GL2 parentGL, String filename)
	{
		try
		{
			GL = parentGL;
			image = new Image(filename);
			initialized = image.Load();
		}
		catch (Exception any)
		{
			System.out.println("..exception loading texture " + any.getMessage());
		}
	}
	
	public Boolean Initialized() { return initialized; } 
	
	public void SetColorKey(char Red, char Green, char Blue)
	{
		
	}
	
	public int Height() { return height; }
	public int Width() { return width; }
	public int ID() { return texture_id; }
	
	public void Gen()
	{
		textureBuffers = new int[1];
		GL.glGenTextures(1, textureBuffers, 0);
		texture_id = textureBuffers[0];
	}
	
	public Boolean Rotate()
	{
		Boolean ok = image.Rotate90();
		Bind();
		TextureImage2D();
		return ok;
	}
	
	public void Bind() 
	{
		GL.glBindTexture(GL2.GL_TEXTURE_2D, texture_id);

		// Setup wrap mode
		GL.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_WRAP_S, GL2.GL_CLAMP_TO_EDGE);
		GL.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_WRAP_T, GL2.GL_CLAMP_TO_EDGE);

		// Setup texture scaling filtering
		GL.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MIN_FILTER, GL2.GL_NEAREST);
		GL.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MAG_FILTER, GL2.GL_NEAREST);

		// Send texel data to OpenGL -- nb. notify gl the data is tightly packed,
		// else gl assumes width of image will be divisible by 2nd arg to glPixelStorei and either
		// fail to load the image when not divisible or load a distorted image, the 1 packing avoids these issues.
		GL.glPixelStorei(GL2.GL_UNPACK_ALIGNMENT, 1);
	}
	
	public void TextureImage2D()
	{
		if (!Initialized() || image.pixelData.capacity() == 0) return;
		
		int rgb8_type = (image.components == 4 ? GL2.GL_RGBA8 : image.components == 3 ? GL2.GL_RGB8 : image.components == 2 ? GL2.GL_R8 : -1);
		int rgb_type = (image.components == 4 ? GL2.GL_RGBA : image.components == 3 ? GL2.GL_RGB : image.components == 2 ? GL2.GL_R : - 1);
		if (rgb8_type < 0 || rgb_type < 0) return;	
		
		GL.glTexImage2D(GL2.GL_TEXTURE_2D, 0, rgb8_type, image.Width(), image.Height(), 0, rgb_type, GL2.GL_UNSIGNED_BYTE, image.pixelData);
	}
	
}