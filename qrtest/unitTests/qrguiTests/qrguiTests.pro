TARGET = qrgui_unittests

CONFIG += c++11

LIBS += -L $$PWD/../../../bin -lqrkernel -lqslog -lqrutils -lqrrepo \
		-lmodels -lcontroller -lpluginManager -ltoolPluginInterface

INCLUDEPATH += \
	# A little hack to make .ui files happy. They include other files by relative path based on qrgui/.ui \
	$$PWD/../../../qrgui/icons \
	$$PWD/../../../qrgui/ \

include(../common.pri)

include(modelsTests/modelsTests.pri)

include(helpers/helpers.pri)
