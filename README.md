# Computacao-Grafica-2015-2
Repositório para os projetos da disciplina de Computação Gráfica da UFSC do semestre 2015-2

Compilação Projeto 1:
```
g++ `pkg-config --cflags gtk+-3.0` -o exec -Iinclude/ -I../Include/ *.cpp src/*.cpp `pkg-config --libs gtk+-3.0` -std=c++11 -rdynamic
./exec
```
