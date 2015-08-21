#ifndef DISPLAYFILE_HPP
#define DISPLAYFILE_HPP

#include "ListaEnc.hpp"
#include "Objects.hpp"

class DisplayFile : public ListaEnc<Object*>
{
    public:
        DisplayFile(){}
        virtual ~DisplayFile() {}

        Object* getObj(int pos){ return getFromPos(pos); }
        Object* getObj(Object *obj);

        void addObj(Object *obj);
        void removeObj(Object *obj){ delete retiraEspecifico(obj); }
        int size(){ return getSize(); }
    protected:
    private:
};

void DisplayFile::addObj(Object* obj){
    if(obj->getName() == "")
        throw "Adicione um nome para este objeto.\n";

    if(!contem(obj)){
        adiciona(obj);
    }else
        throw "Ja existe um objeto com este mesmo nome.\n";
}

Object* DisplayFile::getObj(Object *obj){
    return *posicaoMem(obj);
}

#endif // DISPLAYFILE_HPP
