# Simple Makefile for the neural network unit tests
# Enable C++ iostreams
# Tell the code to compile the activation function LUTs used by the unit tests
menu_ut: menu_ut.cpp ../src/menu.cpp
#	Make an output dir to hold the executable
	mkdir -p ./output
#	Build the unit tests
	g++ -g -Wall -o ./output/menu_ut -I../inc menu_ut.cpp  ../src/fonts.cpp  -DOLED_SSD1306 -Wformat=0
clean:
	rm -drf ./output/*
