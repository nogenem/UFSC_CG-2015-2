#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Objects.hpp"
#include "MyException.hpp"

// Valores minimo e maximo que a window pode assumir
#define MIN_SIZE 1
#define MAX_SIZE 5000

class Window
{
    public:
        Window(double vWidth, double vHeight):
            _center( (vWidth/2),(vHeight/2) ), _angle(0),
            _width( (vWidth/2) ), _height( (vHeight/2) ) {}
        virtual ~Window() {}

        Transformation& getT(){ return _t; }
        Coordinate wMin() const { return Coordinate(-1,-1); }
        Coordinate wMax() const { return Coordinate(1,1); }
        Coordinate center(){ return _center; }

        double getWidth(){return _width;}
        double getHeight(){return _height;}
        double getAngulo(){ return _angle; }

        void setAngulo(double graus){ _angle += graus; }

        void zoom(double step);
        void move(double x, double y);
        void moveTo(Coordinate center);

        void updateMatrix();
    protected:
    private:
        Coordinate _center;
        double _angle;
        double _width, _height;
        Transformation _t;
};

void Window::updateMatrix(){
    Coordinate center = this->center();
    _t = Transformation();
    _t *= Transformation::newTranslation(-center.x, -center.y);
    _t *= Transformation::newRotationAroundPoint(-_angle, center);// ta certo?
    _t *= Transformation::newScaling(1/getWidth(), 1/getHeight());
}

void Window::zoom(double step){
    //step /= 2;

    if( (getWidth()+step <= MIN_SIZE || getHeight()+step <= MIN_SIZE) && step < 0 )
        throw MyException("Zoom maximo alcancado.\n");
    else if( (getWidth()+step >= MAX_SIZE || getHeight()+step >= MAX_SIZE) && step > 0)
        throw MyException("Zoom minimo alcancado.\n");

    _width += step;
    _height += step;
}

void Window::move(double x, double y){
    Coordinate c(x,y);
    c *= Transformation::newRotation(_angle);
    _center.x += c.x;
    _center.y += c.y;
}

void Window::moveTo(Coordinate center){
    move(center.x - _center.x, center.y - _center.y);
    _width = 100;
    _height = 100;
}

#endif // WINDOW_HPP
