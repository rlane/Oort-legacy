using GL;
using Math;
using Vector;

namespace Oort {
	public class Texture {
		public GLuint id;

		public Texture() {
			glGenTextures(1, &id);
		}
		
		public void bind() {
			glBindTexture(GL_TEXTURE_2D, id);
		}
	}
}

public class Oort.ParticleTexture : Oort.Texture {
	public ParticleTexture() {
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
				var i = n*y+x;
				Vec2 point = vec2((double)x/n, (double)y/n);
				double dist = vec2(0.5,0.5).distance(point);
				double alpha = Math.pow(1-(2*dist).clamp(0, 1), 2);
				data[i] = (uint8) (alpha*255);
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, n, n, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
