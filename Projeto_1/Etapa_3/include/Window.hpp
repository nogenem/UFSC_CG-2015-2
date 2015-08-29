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
            _wmin(-1,-1), _wmax(1,1), _angle(0) {}
        virtual ~Window() {}

        Transformation& getT(){ return _t; }
        Coordinate wMin() const { return _wmin; }
        Coordinate wMax() const { return _wmax; }
        void setAngulo(double graus){ _angle += graus; }

        void zoom(double step);
        void moveX(double v){ _wmin.x += v; _wmax.x += v;}
        void moveY(double v){ _wmin.y += v; _wmax.y += v;}
        void moveTo(Coordinate center);

        void updateMatrix();
    protected:
    private:
        Coordinate _wmin, _wmax;
        double _angle;
        Transformation _t;

        double getWidth(){return _wmax.x-_wmin.x;}
        double getHeight(){return _wmax.y-_wmin.y;}
        Coordinate center(){ return Coordinate(getWidth()/2, getHeight()/2); }
};

void Window::updateMatrix(){
    Coordinate center = this->center();
    _t = Transformation();
    _t *= Transformation::newTranslation(-center.x, -center.y);
    _t *= Transformation::newRotation(_angle);
    _t *= Transformation::newScaling(1/getWidth(), 1/getHeight());
}

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

void Window::moveTo(Coordinate center){
    this->_wmin = center;
    this->_wmin -= 1;
    this->_wmax = center;
    this->_wmax += 1;
}

#endif // WINDOW_HPP
