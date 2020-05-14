#ifndef PLANE_H
#define PLANE_H

class Plane
{
private:
	unsigned int vaoHandle;
	int faces;

public:
	Plane(float, float, int, int);

	void render() const;
};

#endif // PLANE_H