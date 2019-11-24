gcc -fPIC -c tModules.c -o tModules.o

gcc -fPIC -c CounterModule.c -o CounterModule.o
gcc -shared CounterModule.o tModules.o -o cntmod.so

gcc -fPIC -c ButtonModule.c -o ButtonModule.o 
gcc -shared ButtonModule.o tModules.o -o btnmod.so

gcc -c main1.c -o main1.o
gcc -c main2.c -o main2.o
gcc -c main3.c -o main3.o

gcc main1.o tModules.o -o test01 -ldl -lSDL2 -lSDL2_image -lSDL2_ttf
gcc main2.o tModules.o -o test02 -ldl -lSDL2 -lSDL2_image -lSDL2_ttf
gcc main3.o tModules.o -o test03 -ldl -lSDL2 -lSDL2_image -lSDL2_ttf

