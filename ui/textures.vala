using GL;
using Math;

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
