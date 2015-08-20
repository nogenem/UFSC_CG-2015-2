#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Objects.hpp"

// Valores minimo e maximo que a window pode assumir
#define MIN_SIZE 1
#define MAX_SIZE 10000

class Window
{
    public:
        Window(double vWidth, double vHeight):
            _wmin(0,0), _wmax(vWidth,vHeight),
            _initWidth(vWidth), _initHeight(vHeight) {}
        virtual ~Window() {}

        Coordinate wMin() const { return _wmin; }
        Coordinate wMax() const { return _wmax; }
        void zoom(double step);
        void move(double x, double y);
    protected:
    private:
        Coordinate _wmin, _wmax;
        double _initWidth, _initHeight;

        double getWidth(){return _wmax.x-_wmin.x;}
        double getHeight(){return _wmax.y-_wmin.y;}
};

void Window::zoom(double step){
    step /= 2;

    _wmin += step;
    _wmax -= step;

    if( (getWidth() <= MIN_SIZE || getHeight() <= MIN_SIZE) && step > 0 ){
        _wmin -= step;//volta ao estado anterior
        _wmax += step;
        throw "Zoom maximo alcancado.\n";
    }else if( (getWidth() >= MAX_SIZE || getHeight() >= MAX_SIZE) && step < 0){
        _wmin -= step;//volta ao estado anterior
        _wmax += step;
        throw "Zoom minimo alcancado.\n";
    }
}

void Window::move(double x, double y){
    _wmin.x += x;
    _wmin.y += y;
    _wmax.x += x;
    _wmax.y += y;
}

#endif // WINDOW_HPP
