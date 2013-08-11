# параметры сборки, специфичные для окружения
win32:BOOST_PATH = E:\\boost_1_54_0

# общие настройки
TARGET = wbc
CONFIG += console
CONFIG += warn_on
CONFIG -= qt

# файлы проекта
HEADERS += \
	source/thewizardplusplus/wizard_basic/exceptions/WizardBasicException.h \
	source/thewizardplusplus/wizard_basic/parser/exceptions/CodePosition.h \
	source/thewizardplusplus/wizard_basic/parser/exceptions/ParsingException.h \
	source/thewizardplusplus/wizard_basic/parser/WizardBasicGrammarRule.h \
	source/thewizardplusplus/wizard_basic/parser/UnsignedRealParserPolicies.h \
	source/thewizardplusplus/wizard_basic/parser/WizardBasicGrammar.h \
	source/thewizardplusplus/wizard_basic/parser/Parser.h
SOURCES += \
	source/thewizardplusplus/wizard_basic/exceptions/WizardBasicException.cpp \
	source/thewizardplusplus/wizard_basic/parser/exceptions/CodePosition.cpp \
	source/thewizardplusplus/wizard_basic/parser/exceptions/ParsingException.cpp \
	source/thewizardplusplus/wizard_basic/parser/WizardBasicGrammarRule.cpp \
	source/thewizardplusplus/wizard_basic/parser/Parser.cpp \
	source/thewizardplusplus/wizard_basic/main.cpp

# файлы внешних библиотек
win32 {
	INCLUDEPATH += $$BOOST_PATH
}

# флаги компилятора
unix | win32-g++ {
	QMAKE_CXXFLAGS += -std=c++98 -pedantic -Wall -W -O3
}
win32-g++ {
	QMAKE_CXXFLAGS += -U__STRICT_ANSI__
}
win32-msvc2010 {
	QMAKE_CXXFLAGS += -W3 -Ox
}
