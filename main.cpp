#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "disp.h"
#include "prog.h"
#include "util.h"

static constexpr glm::vec3 col[2] = {
	glm::vec3(38, 38, 38),
	glm::vec3(255, 142, 65)
};

std::vector<std::string> split(std::string buff, char delim) {
	std::vector<std::string> tok;

	std::stringstream s(buff);
	std::string seg;
	while (std::getline(s, seg, delim)) {
		tok.push_back(seg);
	}

	return tok;
}

std::vector<std::string> rd(std::string fName) {
	std::ifstream in;
	in.open(fName);

	std::vector<std::string> cont;

	for (std::string l; std::getline(in, l);) {
		cont.push_back(l);
	}

	in.close();

	return cont;
}

std::vector<GLfloat> rdAttr(std::string fName, unsigned int attr) {
	std::vector<GLfloat> _;

	std::vector<std::string> buff = rd(fName + ".obj");

	const std::string id[3] = {
		"v",
		"vt",
		"vn"
	};
	const unsigned int sz[3] = {
		3,
		2,
		3
	};

	for (int l = 0; l < buff.size(); l++) {
		std::vector<std::string> tok = split(buff[l], ' ');

		if (tok[0] == id[attr]) {
			for (int i = 1; i < 1 + sz[attr]; i++) {
				std::stringstream out;
				out << std::fixed << std::setprecision(4) << std::stof(tok[i]);

				_.push_back(std::stof(out.str()));
			}
		}
	}

	return _;
}

int main() {
	Disp disp("asdf", 1000, 1000);

	// data
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// position
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	std::vector<GLfloat> vtc = rdAttr("a", 0);
	glBufferData(GL_ARRAY_BUFFER, vtc.size() * sizeof (GLfloat), &vtc[0], GL_STATIC_DRAW);

	// matrix
	const GLfloat scaleFac = 0.1;

	glm::mat4 model = glm::mat4(1.0);
	model = glm::scale(model, glm::vec3(scaleFac, scaleFac, scaleFac));

	// shader
	Prog prog("shad", "shad");

	/// attribute
	GLint attrPos = glGetAttribLocation(prog._id, "pos");
	glVertexAttribPointer(attrPos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) 0);
	glEnableVertexAttribArray(attrPos);

	/// uniform
	GLint uniModel = glGetUniformLocation(prog._id, "model");

	// initialize
	prog.use();

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	SDL_Event e;
	while (disp.open) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				SDL_Quit();
			}
		}

		disp.clear(col[false].r / 255.0, col[false].g / 255.0, col[false].b / 255.0, 1);

		glDrawArrays(GL_TRIANGLES, 0, vtc.size() / 3);

		disp.update();
	}
}
