import java.nio.ByteBuffer;
import com.jogamp.opengl.GL2;

public class FrameHandler
{
	public static JPEG image;
	public static ByteBuffer imageBuffer;
	public static int texture_id;
	public static float aspect;
	public static String currFilename;
	
	FrameHandler()
	{
		
	}

	public static boolean load(String[] filenames)
	{
		System.out.println("..load multiple filenames not implemented!");
		return false;
	}
	
	public static boolean load(GL2 gl2, String filename)
	{	
		currFilename = filename;
		String extension = StringUtils.extension(filename);
		if (extension.equals(".jpeg") || extension.equals(".jpg"))
		{
			image = new JPEG();
			int prev_id = texture_id;
			try
			{
				if (image.load(filename))
				{
					imageBuffer = image.data().getByteBuffer(0, image.size());
					imageBuffer.position(0);
					aspect = (float) image.width() / ( float) image.height();
					
					int[] renderBuffers = new int[1];
					gl2.glGenTextures(1, renderBuffers, 0);
					texture_id = renderBuffers[0];

					gl2.glBindTexture(GL2.GL_TEXTURE_2D, texture_id);

					// Setup wrap mode
					gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_WRAP_S, GL2.GL_CLAMP_TO_EDGE);
					gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_WRAP_T, GL2.GL_CLAMP_TO_EDGE);

					// Setup texture scaling filtering
					gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MIN_FILTER, GL2.GL_NEAREST);
					gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MAG_FILTER, GL2.GL_NEAREST);

					// Send texel data to OpenGL -- nb. notify gl the data is tightly packed,
					// else gl assumes width of image will be divisible by 2nd arg to glPixelStorei and either
					// fail to load the image when not divisible or load a distorted image, the 1 packing avoids these issues.
					gl2.glPixelStorei(GL2.GL_UNPACK_ALIGNMENT, 1);
					glTexImage2D(gl2, image.components());
				}
			}
			catch (Exception any)
			{
				texture_id = prev_id;
				gl2.glBindTexture(GL2.GL_TEXTURE_2D, texture_id);
				
				System.out.println("..image load failed exception: " + any.getMessage());
			}
		}
		return true;
	}	
	
	private static void glTexImage2D(GL2 gl2, int components)
	{
		if (imageBuffer == null || imageBuffer.capacity() == 0) return;
		int rgb8_type = (components == 4 ? GL2.GL_RGBA8 : components == 3 ? GL2.GL_RGB8 : components == 2 ? GL2.GL_R8 : -1);
		int rgb_type = (components == 4 ? GL2.GL_RGBA : components == 3 ? GL2.GL_RGB : components == 2 ? GL2.GL_R : - 1);
		if (rgb8_type < 0 || rgb_type < 0) return;	
		System.out.println("..dbg components = " + components + " width = " + image.width() + " height = " + image.height() + " sz = " + image.size() + " buffer.cap() = " + imageBuffer.capacity());
		System.out.println("..dbg rbg8_type = " + rgb8_type + " rgb_type = " + rgb_type); 
		
		gl2.glTexImage2D(GL2.GL_TEXTURE_2D, 0, rgb8_type, image.width(), image.height(), 0, rgb_type, GL2.GL_UNSIGNED_BYTE, imageBuffer);
		
		//gl2.glGenerateMipmap(GL2.GL_TEXTURE_2D);
		//gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MIN_FILTER, GL2.GL_LINEAR_MIPMAP_LINEAR);
		//gl2.glTexParameteri(GL2.GL_TEXTURE_2D, GL2.GL_TEXTURE_MAG_FILTER, GL2.GL_LINEAR);
	}
	
	public static boolean save(String filename)
	{
		String extension = StringUtils.extension(filename);
		if (extension.equals(".jpeg") || extension.equals(".jpg"))
		{
			if (image.save(filename, 10))
			{
				System.out.println("..saved jpeg image ["+filename+"].");
			}
		}
		return true;
	}
	
	
}
