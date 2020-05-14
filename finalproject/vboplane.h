#ifndef VBOPLANE_H
#define VBOPLANE_H

class VBOPlane 
{
private:
    unsigned int vaoHandle;
    int faces;

public:
    VBOPlane(float, float, int, int);

    void render() const;
};

#endif // VBOPLANE_H
