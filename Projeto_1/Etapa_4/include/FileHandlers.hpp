#ifndef FILEHANDLERS_HPP
#define FILEHANDLERS_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <string>

/*
    Possiveis Diretivas:
        v, o, p, l, f,
        usemtl, newmtl, Kd

    Obs:
        Index dos vertices podem ser positivos
         ou negativos.

        Os arquivos .mtl devem estar na mesma
         pasta que o arquivo .obj.
*/

class ColorReader
{
    public:
        ColorReader(){}
        bool loadFile(std::string filename);
        GdkRGBA getColor(std::string colorName);
    private:
        //Ex: <"Color0", GdkRGBA(0,0.6,0)>
        std::map<std::string, GdkRGBA> _colors;
        std::ifstream _ifs;

        void loadColors();
        void addColor(std::string name, std::stringstream& line);
};

class ColorWriter
{
    public:
        ColorWriter(): _numColors(0) {}
        void loadFile(std::string filename);
        std::string getColorName(GdkRGBA color);
    private:
        //Ex: <"(0,0.6,0)", "Color0">
        std::map<std::string, std::string> _colors;
        std::ofstream _ofs;
        //Contador de cores ja escritas, usado no nome da cor
        int _numColors;

        void writeColor(std::string colorName, GdkRGBA& color);
};

class ObjStream
{
    public:
        ObjStream(std::string& filename);
    protected:
        std::string _name;// Nome do arquivo, sem a extensao
        std::string _path;// Path para o arquivo
        std::fstream _objsFile;
        std::fstream _colorsFile;
        GdkRGBA _color;// Cor atual
};

class ObjReader : public ObjStream
{
    public:
        ObjReader(std::string& filename);
        std::vector<Object*>& getObjs(){ return _objs; }
    private:
        std::vector<Object*> _objs;
        Coordinates _coords;// Todas as coordenadas lidas do arquivo
        ColorReader _cReader;
        bool _usingColorsFile;// Existe uma chamada 'mtllib' no .obj?
        int _numSubObjs; // Numero de objetos em seguida com o mesmo nome

        void loadObjs();
        void loadColorsFile(std::stringstream& line);
        void changeColor(std::stringstream& line);
        void addCoord(std::stringstream& line);
        void addPoint(std::stringstream& line);
        void addPoly(std::stringstream& line, bool filled);
        // Usado para destruir os objs caso de algum erro
        void destroyObjs();
};

class ObjWriter : public ObjStream
{
    public:
        ObjWriter(std::string& filename);
        void writeObjs(World *world);
    private:
        // Numero de coordenadas ja escritas
        int _numVertex;
        ColorWriter _cWriter;

        void printObj(Object* obj);
};

ObjStream::ObjStream(std::string& filename): _color({0}) {
    if(!filename.find("\\.obj"))
        filename += ".obj";

    std::size_t found = filename.find_last_of("/\\");
    _path = filename.substr(0,found+1);
    _name = filename.substr(found+1, filename.size()-found-5);// Nome sem o '.obj'
}

ObjReader::ObjReader(std::string& filename):
    ObjStream(filename){

    _numSubObjs = 0;
    _usingColorsFile = false;
    _objsFile.open(filename.c_str());
    if(!_objsFile.is_open())
        throw MyException("Erro tentando abrir o arquivo:\n\t" + filename + ".\n");
    else
        loadObjs();
}

void ObjReader::destroyObjs(){
    for(auto o : _objs)
        delete o;
}

void ObjReader::loadObjs(){
    std::string tmp, keyWord;
    while(std::getline(_objsFile, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "#"){ /* Nao faz nada... */ }
        else if(keyWord == "mtllib"){ loadColorsFile(line); }
        else if(keyWord == "usemtl"){ if(_usingColorsFile){changeColor(line);} }
        else if(keyWord == "v"){ addCoord(line); }
        else if(keyWord == "o"){ line >> _name; _numSubObjs = 0; }
        else if(keyWord == "w"){ /* TODO(?) */ }
        else if(keyWord == "p"){ addPoint(line); _numSubObjs++; }
        else if(keyWord == "l"){ addPoly(line, false); _numSubObjs++; }
        else if(keyWord == "f"){ addPoly(line, true); _numSubObjs++; }
    }
}

void ObjReader::loadColorsFile(std::stringstream& line){
    std::string file;
    line >> file;
    _usingColorsFile = _cReader.loadFile(_path+file);
}

void ObjReader::changeColor(std::stringstream& line){
    std::string colorName;
    line >> colorName;
    _color = _cReader.getColor(colorName);
}

void ObjReader::addCoord(std::stringstream& line){
    double x,y;
    line >> x >> y;
    _coords.emplace_back(x,y);
}

void ObjReader::addPoint(std::stringstream& line){
    int index = 0;
    int size = _coords.size();

    line >> index;

    if(index < 0)
        index = size + (index+1);
    else
        index--;

    if(index < 0 || index >= size){
        destroyObjs();
        throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
    }

    std::string name = _numSubObjs == 0 ? _name : _name+"_sub"+std::to_string(_numSubObjs);
    Point *p = new Point(name, _color);
    p->addCoordinate(_coords[index]);
    _objs.push_back(p);
}

void ObjReader::addPoly(std::stringstream& line, bool filled){
    int index = 0;
    int size = _coords.size();
    Coordinates objCoords;

    while(line >> index){
        if(index < 0)
            index = size + index;
        else
            index--;

        if(index < 0 || index >= size){
            destroyObjs();
            throw MyException("Indice de vertice invalido na linha: "+ line.str() + ".\n");
        }

        objCoords.push_back(_coords[index]);
    }

    Object *obj;
    std::string name = _numSubObjs == 0 ? _name : _name+"_sub"+std::to_string(_numSubObjs);
    if(objCoords.size() == 2)
        obj = new Line(name, _color, objCoords);
    else
        obj = new Polygon(name, _color, filled, objCoords);

    _objs.push_back(obj);
}

ObjWriter::ObjWriter(std::string& filename):
    ObjStream(filename) {

    _numVertex = 0;
    _objsFile.open(filename.c_str(), std::fstream::out);

    if(!_objsFile.is_open())
        throw MyException("Erro tentando abrir arquivo:\n\t" + filename + ".\n");
}

void ObjWriter::writeObjs(World *world){
    _cWriter.loadFile(_path+_name+".mtl");
    _objsFile << "mtllib " << _name << ".mtl\n\n";

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
        _objsFile << "v " << c.x << " " << c.y << "\n";

    _objsFile << "\no " << obj->getName() << "\n";

    std::string colorName = _cWriter.getColorName(obj->getColor());

    if(colorName != "none")
        _objsFile << "usemtl " << colorName << "\n";

    std::string keyWord;
    switch(obj->getType()){
    case ObjType::POINT:
        keyWord = "p";
        break;
    case ObjType::LINE:
        keyWord = "l";
        break;
    case ObjType::POLYGON:
        keyWord = ((Polygon*)obj)->filled() ? "f" : "l";
        break;
    default:
        break;
    }

    int size = coords.size();
    _objsFile << keyWord;
    for(int i = 0; i<size; i++)
        _objsFile << " " << _numVertex+(i+1);

    _objsFile << "\n" << std::endl;
    _numVertex += size;
}

bool ColorReader::loadFile(std::string filename){
    _ifs.open(filename);

    if(!_ifs.is_open()){ //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
        return false;
    }else{
        loadColors();
        return true;
    }
}

void ColorReader::loadColors(){
    std::string tmp, keyWord, colorName;
    while(std::getline(_ifs, tmp)){
        if(tmp.size() <= 1) continue;
        std::stringstream line(tmp);
        line >> keyWord;

        if(keyWord == "newmtl"){ line >> colorName; }
        if(keyWord == "Kd"){ addColor(colorName, line); }
    }
}

void ColorReader::addColor(std::string name, std::stringstream& line){
    double r,g,b;
    line >> r >> g >> b;
    GdkRGBA color{r,g,b};
    _colors[name] = color;
}

GdkRGBA ColorReader::getColor(std::string colorName){
    auto iter = _colors.find(colorName);
    if(iter != _colors.end())
        return iter->second;
    else
        return GdkRGBA({0});
}

void ColorWriter::loadFile(std::string filename){
    _ofs.open(filename);

    if(!_ofs.is_open()) //dar throw?
        std::cout << "Erro tentando abrir arquivo:\n\t" + filename + ".\n";
}

std::string ColorWriter::getColorName(GdkRGBA color){
    if(!_ofs.is_open())
        return "none";

    std::string colorString = gdk_rgba_to_string(&color);

    auto iter = _colors.find(colorString);
    if(iter != _colors.end())
        return iter->second;
    else{ //Se nao existir, cria um novo nome para a cor, e retorna ele...
        std::string colorName = "Color" + std::to_string(_numColors++);

        _colors[colorString] = colorName;
        writeColor(colorName, color);
        return colorName;
    }
}

void ColorWriter::writeColor(std::string colorName, GdkRGBA& color){
    _ofs << "newmtl " << colorName << "\n";
    _ofs << "Kd " << color.red << " " << color.green << " " << color.blue << "\n\n";
}

#endif // FILEHANDLERS_HPP
