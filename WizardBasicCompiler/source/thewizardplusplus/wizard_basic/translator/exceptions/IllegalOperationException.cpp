#include "IllegalOperationException.h"
#include <boost/format.hpp>

using namespace thewizardplusplus::wizard_basic::translator::exceptions;
using namespace boost;

IllegalOperationException::IllegalOperationException(const std::string&
	operation_description, ValueType::Types operand1_type, ValueType::Types
	operand2_type)
:
	TranslateException((format("illegal %1% for types %2% and %3%") %
		operation_description % ValueType::convertToString(operand1_type) %
		ValueType::convertToString(operand2_type)).str())
{}
