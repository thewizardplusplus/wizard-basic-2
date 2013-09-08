#include "Program.h"
#include "exceptions/DuplicateLabelsException.h"
#include "exceptions/IncorrectOrderOfLabelsException.h"
#include "NumberVariable.h"
#include "ArrayVariable.h"
#include "StringVariable.h"
#include "exceptions/IllegalAssignOperationException.h"
#include "../utils/assert.h"
#include "ArrayAccessExpression.h"
#include "exceptions/IllegalConditionOperationException.h"
#include "FunctionCallExpression.h"
#include "exceptions/JumpsToNonexsistentLabelsExceptions.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <typeinfo>

using namespace thewizardplusplus::wizard_basic::compiler::translator;
using namespace thewizardplusplus::wizard_basic::compiler::translator::
	exceptions;
using namespace boost;

const VariableList& Program::getVariables(void) const {
	return variables;
}

FunctionList& Program::getFunctions(void) {
	return functions;
}

void Program::addLabel(size_t label) {
	if (std::find(labels.begin(), labels.end(), label) != labels.end()) {
		throw DuplicateLabelsException();
	}
	if (labels.size() != 0 && label < labels.back()) {
		throw IncorrectOrderOfLabelsException();
	}
	labels.push_back(label);

	cpp_code += (format("\tlabel%1%:\n") % label).str();
}

void Program::addVariable(const Variable::Pointer& variable) {
	variables.addVariable(variable);
}

void Program::addAssign(const std::string& variable_name, const shared_ptr<
	Expression>& expression)
{
	Variable::Pointer variable = variables.getVariableByName(variable_name);
	if (!variable) {
		variable = Variable::Pointer(new NumberVariable(variable_name));
		variables.addVariable(variable);
	}
	if (typeid(*variable) != typeid(NumberVariable) || expression->getType() !=
		ValueType::NUMBER)
	{
		ValueType::Types variable_type = ValueType::UNKNOWN_TYPE;
		if (typeid(*variable) == typeid(NumberVariable)) {
			variable_type = ValueType::NUMBER;
		} else if (typeid(*variable) == typeid(ArrayVariable) || typeid(
			*variable) == typeid(StringVariable))
		{
			variable_type = ValueType::ARRAY;
		}

		throw IllegalAssignOperationException(variable_type, expression->
			getType());
	}

	cpp_code += (format("\t\t%1% = %2%;\n") % variable_name % expression->
		getCppCode()).str();
}

void Program::addAssign(const Expression::Pointer& array_access, const
	Expression::Pointer& expression)
{
	ASSERT(typeid(*array_access) == typeid(ArrayAccessExpression), "Wizard "
		"BASIC: translating error - invalid expression type; expected "
		"ArrayAccessExpression.");

	if (array_access->getType() != expression->getType()) {
		throw IllegalAssignOperationException(array_access->getType(),
			expression->getType());
	}

	cpp_code += (format("\t\t%1% = %2%;\n") % array_access->getCppCode() %
		expression->getCppCode()).str();
}

void Program::addJump(size_t label) {
	jumps.push_back(label);
	cpp_code += (format("\t\tgoto label%1%;\n") % label).str();
}

void Program::addCondition(const Expression::Pointer& left_expression,
	ConditionType::Types condition_type, const Expression::Pointer&
	right_expression, size_t label)
{
	if (left_expression->getType() != right_expression->getType() ||
		(left_expression->getType() != ValueType::NUMBER && left_expression->
		getType() != ValueType::ARRAY))
	{
		throw IllegalConditionOperationException(left_expression->getType(),
			right_expression->getType());
	}

	jumps.push_back(label);
	cpp_code += (format(
		"\t\tif (%1% %2% %3%) {\n"
		"\t\t\tgoto label%4%;\n"
		"\t\t}\n"
	) % left_expression->getCppCode() % ConditionType::convertToCppCode(
		condition_type) % right_expression->getCppCode() % label).str();
}

void Program::addFunctionCallAsStatement(const Expression::Pointer&
	function_call)
{
	ASSERT(typeid(*function_call) == typeid(FunctionCallExpression), "Wizard "
		"BASIC: translating error - invalid expression type; expected "
		"FunctionCallExpression.");

	cpp_code += (format("\t\t%1%;\n") % function_call->getCppCode()).str();
}

std::string Program::getCppCode(void) const {
	testJumps();

	std::string code = cpp_code;
	LabelVector unused_labels = findUnusedLabels();
	LabelVector::const_iterator i = unused_labels.begin();
	for (; i != unused_labels.end(); ++i) {
		erase_all(code, (format("\tlabel%1%:\n") % *i).str());
	}

	return (format(
		"#include <WizardBasicFramework/base/Array.h>\n"
		"#include <WizardBasicFramework/system/System.h>\n"
		"#include <iostream>\n"
		"\n"
		"using namespace thewizardplusplus::wizard_basic::framework::base;\n"
		"using namespace thewizardplusplus::wizard_basic::framework::system;\n"
		"\n"
		"int main(void) {\n"
		"%1%"
		"\ttry {\n"
		"%2%"
		"\t} catch (const OutOfBoundsException& exception) {\n"
		"\t\tstd::cerr << \"Wizard BASIC: runtime error - \" << exception."
			"what() << \".\" << std::endl;\n"
		"\t}\n"
		"}\n"
	) % variables.getCppDefinition() % code).str();
}

void Program::testJumps(void) const {
	LabelSet jumps(this->jumps.begin(), this->jumps.end());
	LabelSet labels(this->labels.begin(), this->labels.end());

	LabelVector result(jumps.size());
	LabelVector::const_iterator end = std::set_difference(jumps.begin(), jumps.
		end(), labels.begin(), labels.end(), result.begin());
	result.resize(end - result.begin());

	if (!result.empty()) {
		throw JumpsToNonexsistentLabelsExceptions(result);
	}
}

Program::LabelVector Program::findUnusedLabels(void) const {
	LabelSet labels(this->labels.begin(), this->labels.end());
	LabelSet jumps(this->jumps.begin(), this->jumps.end());

	LabelVector result(labels.size());
	LabelVector::const_iterator end = std::set_difference(labels.begin(), labels
		.end(), jumps.begin(), jumps.end(), result.begin());
	result.resize(end - result.begin());

	return result;
}
