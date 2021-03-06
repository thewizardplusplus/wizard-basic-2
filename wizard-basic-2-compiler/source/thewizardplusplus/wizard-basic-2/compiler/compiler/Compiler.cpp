#include "Compiler.h"
#include "exceptions/FailedOpenTemporaryFileException.h"
#include "../utils/os.h"
#include "exceptions/AssemblingOrLinkingException.h"
#include <boost/format.hpp>
#include <cstdio>
#include <fstream>

using namespace thewizardplusplus::wizard_basic_2::compiler::compiler;
using namespace
	thewizardplusplus
	::wizard_basic_2
	::compiler
	::compiler
	::exceptions;
using namespace boost;

void Compiler::compile(
	const std::string& translated_code,
	const std::string& compiler_path,
	const std::string& output_filename
) {
	std::string temporary_filename = std::string(std::tmpnam(NULL)) + ".cpp";
	std::ofstream out(temporary_filename.c_str());
	if (!out.is_open()) {
		throw FailedOpenTemporaryFileException(temporary_filename);
	}
	out << translated_code;
	out.close();

	#ifdef OS_LINUX
		std::string command =
			(format(
				"g++ "
				"-I%2%../framework/headers/ "
				"-o %3% "
				"%1%"
				" -L%2%../framework/libraries/wizard_basic_2_framework/ "
				"-lwb2f -lAnnaGraphics -lX11 -lGL -pthread -lAnnaSound -lopenal"
			)
				% temporary_filename
				% compiler_path
				% output_filename).str();
	#elif defined(OS_WINDOWS)
		std::string command =
			(format(
				"g++ "
				"-I%2%../framework/headers/ "
				"-Wl,-subsystem,console"
				"-o %3% "
				"%1%"
				" -L%2%../framework/libraries/wizard_basic_2_framework/ "
				"-lwb2f -lAnnaGraphics -lopengl32 -lgdi32 -lAnnaSound -lopenal"
			)
				% temporary_filename
				% compiler_path
				% output_filename).str();
	#endif
	int exit_code = std::system(command.c_str());
	if (exit_code) {
		throw AssemblingOrLinkingException();
	}

	remove(temporary_filename.c_str());
}
