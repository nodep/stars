%.o: %.cpp
	c++ -isystem C:\mingw\include\c++\STLport\stlport -O2 -c $(<) -o $*.o

objects = calcs.o config.o constellation_boundary.o coordinate.o\
		main.o process.o ps_canvas.o star.o storage.o text.o

stars.exe : $(objects)
	c++ $(objects) -lstlport_mingw32_static -o stars

calcs.o : calcs.cpp
config.o : config.cpp
constellation_boundary.o : constellation_boundary.cpp
coordinate.o : coordinate.cpp
main.o : main.cpp
process.o : process.cpp
ps_canvas.o : ps_canvas.cpp
star.o : star.cpp
stars_gui.o : stars_gui.cpp
storage.o : storage.cpp
text.o : text.cpp
