#pragma once

#include "GLFW/glfw3.h"

namespace Tactics {

	namespace Util {

		class ShaderProgramHelper {
		public:
			static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
			static GLuint LoadAndUseShader(const char *, const char *);
		private:

		};

	} // namespace Util

} // namespace Tactics
