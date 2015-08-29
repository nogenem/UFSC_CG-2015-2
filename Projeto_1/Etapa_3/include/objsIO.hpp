#ifndef OBJSIO_HPP
#define OBJSIO_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <regex>

class ObjReader
{
    public:
        ObjReader(const std::string filename);
        std::vector<Object*>& getObjs(){ return _objs; }
    private:
        std::string _name;
        std::ifstream _ifs;
        std::vector<Object*> _objs;
        Coordinates _coords;

        void loadObjs();
        void addCoord(std::stringstream& line);
        void addPoint(std::stringstream& line);
        void addPoly(std::stringstream& line);
};

class ObjWriter
{
    public:
        ObjWriter(std::string filename);
        void writeObjs(World *world);
    private:
        std::ofstream _ofs;
        int _numVertex;

        void printObj(Object* obj);
};

ObjReader::ObjReader(const std::string filename): _ifs(filename){
    std::regex e("([^\\\\/]*)\\.obj$");
    std::smatch m;
    std::regex_search (filename,m,e);
    _name = m[1]; //Pega soh o nome do arquivo

    if(!_ifs.is_open())
        throw std::string("Error tring to open file: " + _name + ".\n").c_str();
    else
        loadObjs();
}

void ObjReader::loadObjs(){
    std::string tmp, keyWord;
    while(std::getline(_ifs, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "#"){ continue; }
        if(keyWord == "v"){ addCoord(line); continue; }
        if(keyWord == "o"){ line >> _name; continue; }
        if(keyWord == "w"){ continue; }
        if(keyWord == "p"){ addPoint(line); continue; }
        if(keyWord == "l"){ addPoly(line); continue; }
    }
}

void ObjReader::addCoord(std::stringstream& line){
    double x,y;
    line >> x >> y;
    _coords.emplace_back(x,y);
}

void ObjReader::addPoint(std::stringstream& line){
    int index = 0;
    Point *p = new Point(_name);
    line >> index;

    int size = _coords.size();
    if(index < 0)
        index = size + (index+1);
    else
        index--;

    if(index < 0 || index >= size){
        delete p;
        return;
    }

    p->addCoordinate(_coords[index]);
    _objs.push_back(p);
}

void ObjReader::addPoly(std::stringstream& line){
    int index = 0;
    Polygon *p = new Polygon(_name);

    int size = _coords.size();
    while(line >> index){
        if(index < 0)
            index = size + index;
        else
            index--;

        if(index < 0 || index >= size){
            delete p;
            throw std::string("Invalid vertex index on line: "+ line.str() + ".\n").c_str();
        }

        p->addCoordinate(_coords[index]);
    }

    _objs.push_back(p);
}

ObjWriter::ObjWriter(std::string filename){
    if(!filename.find("\\.obj"))
        filename += ".obj";

    _ofs.open(filename.c_str(), std::ofstream::out);
    _numVertex = 0;

    if(!_ofs.is_open())
        throw std::string("Error tring to open file:\n\t" + filename + ".\n").c_str();
}

void ObjWriter::writeObjs(World *world){
    Object *obj;
    int size = world->numObjs();
    for(int i = 0; i<size; i++){
        obj = world->getObj(i);
        printObj(obj);
    }
}

void ObjWriter::printObj(Object* obj){
    auto coords = obj->getCoords();
    for(const auto &c : coords)
        _ofs << "v " << c.x << " " << c.y << "\n";

    _ofs << "o " << obj->getName() << "\n";

    std::string keyWord;
    switch(obj->getType()){
    case ObjType::POINT:
        keyWord = "p";
        break;
    case ObjType::LINE:
    case ObjType::POLYGON:
        keyWord = "l";
        break;
    default:
        break;
    }

    int size = coords.size();
    _ofs << keyWord;
    for(int i = 0; i<size; i++)
        _ofs << " " << _numVertex+(i+1);

    _ofs << "\n" << std::endl;
    _numVertex += size;
}

#endif // OBJSIO_HPP
