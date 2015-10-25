#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Objects.hpp"
#include "MyException.hpp"

// Valores minimo e maximo que a window pode assumir
#define MIN_SIZE 0.001
#define MAX_SIZE 5000

class Window
{
    public:
        Window(double vWidth, double vHeight):
            m_width( (vWidth/2) ), m_height( (vHeight/2) ) {}
        virtual ~Window() {}

        Transformation& getT(){ return m_t; }
        Coordinate wMin() const { return Coordinate(-1,-1); }
        Coordinate wMax() const { return Coordinate(1,1); }
        Coordinate center(){ return m_center; }

        double getWidth(){return m_width;}
        double getHeight(){return m_height;}
        double getAnguloX(){ return m_angleX; }
        double getAnguloY(){ return m_angleY; }
        double getAnguloZ(){ return m_angleZ; }

        void setAnguloX(double graus){ m_angleX += graus; }
        void setAnguloY(double graus){ m_angleY += graus; }
        void setAnguloZ(double graus){ m_angleZ += graus; }

        void zoom(double step);
        void move(double x, double y, double z=0);
        void moveTo(Coordinate center);

        void updateTransformation();

    private:
        Coordinate m_center;
        double m_angleX = 0, m_angleY = 0,
            m_angleZ = 0;
        double m_width, m_height;
        Transformation m_t;
};

void Window::updateTransformation(){
    m_t = Transformation();
    m_t *= Transformation::newTranslation(-m_center.x, -m_center.y, -m_center.z);
    m_t *= Transformation::newRotation(-m_angleX, -m_angleY, -m_angleZ);
    m_t *= Transformation::newScaling(1.0/m_width, 1.0/m_height, 2.0/(m_width + m_height));//2.0/(m_width + m_height)
}

void Window::zoom(double step){

    step = getWidth()*(step/100);

    if( (getWidth()+step <= MIN_SIZE || getHeight()+step <= MIN_SIZE) && step < 0 )
        throw MyException("Zoom maximo alcancado.\n");
    else if( (getWidth()+step >= MAX_SIZE || getHeight()+step >= MAX_SIZE) && step > 0 )
        throw MyException("Zoom minimo alcancado.\n");

    m_width += step;
    m_height += step;
}

void Window::move(double x, double y, double z){
    Coordinate c(x,y,z);
    c *= Transformation::newRotation(m_angleX, m_angleY, m_angleZ);
    m_center.x += c.x;
    m_center.y += c.y;
    m_center.z += c.z;
}

void Window::moveTo(Coordinate center){
    move(center.x - m_center.x, center.y - m_center.y,
         center.z - m_center.z);

    m_width = 150;
    m_height = 150;
}

#endif // WINDOW_HPP
