#include <vector>

namespace Oort {

template <typename T>
class Bunch {
public:
	GLuint id;
	int size;
	float initial_time;
	static std::vector<GLuint> buffer_freelist;

	Bunch(float initial_time, std::vector<T> data)
		: id(0),
		  size(data.size()),
		  initial_time(initial_time),
		  data(data) {
	}

	~Bunch() {
		if (id != 0) {
			buffer_freelist.push_back(id);
			id = 0;
		}
	}

	void realize() {
		if (buffer_freelist.size() == 0) {
			glGenBuffers(1, &id);
		} else {
			id = buffer_freelist.back();
			buffer_freelist.pop_back();
		}
		bind();
		glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(T), &data[0], GL_STATIC_DRAW);
		unbind();
		data.clear();
	}

	void bind() {
		if (id == 0) {
			realize();
		}
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	static void unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

private:
	std::vector<T> data;
};

};
