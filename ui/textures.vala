using GL;
using Math;

namespace RISC {
	public class Texture {
		public GLuint id;

		public Texture() {
			glGenTextures(1, &id);
		}
		
		public void bind() {
			glBindTexture(GL_TEXTURE_2D, id);
		}
	}

	public class IonBeamTexture : Texture {
		public IonBeamTexture() {
			bind();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			var n = 256;
			var data = new uint8[3*n*n];
			for (int y = 0; y < n; y++) {
				for (int x = 0; x < n; x++) {
					var i = 3*(n*y+x);
					var f = y < 250 ? 1.0 : (255-y)/5.0;
					data[i+0] = (uint8)(0.5*255*f*Math.sin(Math.PI*x/(n-1)));
					data[i+1] = (uint8)(0.5*255*f*Math.sin(Math.PI*x/(n-1)));
					data[i+2] = (uint8)(255*f*Math.sin(Math.PI*x/(n-1)));
				}
			}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, n, n, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}
