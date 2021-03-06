#include "ArrayVariable.h"
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_basic_2::compiler::translator;
using namespace boost;

ArrayVariable::ArrayVariable(const std::string& name, size_t size) :
	Variable(name),
	size(size)
{}

std::string ArrayVariable::getCppDefinition(void) const {
	return (format("\tWIZARD_BASIC_ARRAY(%1%, %2%);\n") % name % size).str();
}
