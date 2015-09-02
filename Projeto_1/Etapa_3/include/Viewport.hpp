#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <ctime>
#include "Window.hpp"
#include "Objects.hpp"
#include "World.hpp"

#define PI 3.1415926535897932384626433832795

class Viewport
{
    public:
        Viewport(double width, double height, World *world):
            _width(width), _height(height), _world(world), _window(width,height),
            _border(new Border(width,height)) { transformAllObjs(); }
        virtual ~Viewport(){ delete _border; }

        Coordinate transformCoordinate(const Coordinate& c) const;
        Coordinates transformCoordinates(const Coordinates& coords) const;

        void transformObj(Object* obj);

        void gotoObj(std::string objName);
        void zoomWindow(double step){_window.zoom(step); transformAllObjs();}
        void moveWindow(double x, double y){_window.move(x,y); transformAllObjs();}
        void rotateWindow(double graus){_window.setAngulo(graus); transformAllObjs();}
        void drawObjs(cairo_t* cr);
    protected:
    private:
        double _width, _height;
        World *_world;
        Window _window;

        cairo_t* _cairo;

        class Border : public Polygon {
            public:
                Border(int width, int height);
        };
        Border *_border;

        void transformAllObjs();

        void drawObj(Object* obj);
        void drawPoint(Object* obj);
        void drawLine(Object* obj);
        void drawPolygon(Object* obj);

        void prepareContext(GdkRGBA& color);
};

// Cria a borda da Viewport
Viewport::Border::Border(int width, int height) :
    Polygon("_border_", GdkRGBA({0,0.9,0})) {
    addCoordinate(0,0);
    addCoordinate(width, 0);
    addCoordinate(width, height);
    addCoordinate(0, height);
}

// Remover isso?
void Viewport::gotoObj(std::string objName){
    Object *obj = _world->getObj(objName);
    Coordinate c = obj->center();
    _window.moveTo(c);
    transformAllObjs();
}

void Viewport::transformObj(Object* obj){
    auto t = _window.getT();
    obj->transformNormalized(t);
}

void Viewport::transformAllObjs(){
    _window.updateMatrix();
    auto t = _window.getT();

    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        obj->transformNormalized(t);
    }
    _border->transformNormalized(t);
}

Coordinate Viewport::transformCoordinate(const Coordinate& c) const {
    const Coordinate wmin = _window.wMin();
    const Coordinate wmax = _window.wMax();

    double x = ((c.x-wmin.x)/(wmax.x-wmin.x))*_width;
    double y = (1-((c.y-wmin.y)/(wmax.y-wmin.y)))*_height;

    return Coordinate(x,y);
}

Coordinates Viewport::transformCoordinates(const Coordinates& coords) const{
    Coordinates vCoords;
    for(const auto &c : coords)
        vCoords.push_back(transformCoordinate(c));
    return vCoords;
}

void Viewport::drawObjs(cairo_t* cr){
    _cairo = cr;

    this->drawObj(_border);
    for(int i = 0; i < _world->numObjs(); i++){
        Object *obj = _world->getObj(i);
        this->drawObj(obj);
    }
}

void Viewport::drawObj(Object* obj){
    if(obj->getNCoordsSize() == 0) return;

    switch(obj->getType()){
    case ObjType::OBJECT:
        break;
    case ObjType::POINT:
        this->drawPoint(obj);
        break;
    case ObjType::LINE:
        this->drawLine(obj);
        break;
    case ObjType::POLYGON:
        this->drawPolygon(obj);
        break;
    }
}

void Viewport::drawPoint(Object* obj){
    Coordinate coord = transformCoordinate(obj->getNCoord(0));
    prepareContext(obj->getColor());
    cairo_move_to(_cairo, coord.x, coord.y);
    cairo_arc(_cairo, coord.x, coord.y, 1.0, 0.0, (2*PI) );//pnt deveria ir diminuindo, nao?
    cairo_fill(_cairo);
}

void Viewport::drawLine(Object* obj){
    auto coords = obj->getNCoords();
    if(coords[0] == coords[1]){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }
    coords = transformCoordinates(coords);
    prepareContext(obj->getColor());

    cairo_move_to(_cairo, coords[0].x, coords[0].y);
    cairo_line_to(_cairo, coords[1].x, coords[1].y);
    cairo_stroke(_cairo);
}

void Viewport::drawPolygon(Object* obj){
    auto coords = obj->getNCoords();
    if(coords.size() == 1){// Usuario quer um ponto?
        drawPoint(obj);
        return;
    }else if(coords.size() == 2){// Usuario quer uma linha?
        drawLine(obj);
        return;
    }

    coords = transformCoordinates(coords);
    prepareContext(obj->getColor());

    cairo_move_to(_cairo, coords[0].x, coords[0].y);
    for(unsigned int i = 0; i<coords.size(); i++)
        cairo_line_to(_cairo, coords[i].x, coords[i].y);

    cairo_close_path(_cairo);

    Polygon* p = (Polygon*) obj;
    if(p->filled()){
        cairo_stroke_preserve(_cairo);
        cairo_fill(_cairo);
    }else
        cairo_stroke(_cairo);
}

void Viewport::prepareContext(GdkRGBA& color){
    cairo_set_source_rgb(_cairo, color.red, color.green, color.blue);
    cairo_set_line_width(_cairo, 1);
}

#endif // VIEWPORT_HPP
