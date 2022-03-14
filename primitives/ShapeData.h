#pragma once
#include "Vertex.h"
#include <GL/glew.h>
struct ShapeData
{
	ShapeData() :
		vertices(0), numVerts(0), indices(0), numIndices(0) {}

	Vertex* vertices;
	unsigned int numVerts;
	unsigned short* indices;
	unsigned int numIndices;

	GLsizeiptr vertexBufferSize() const { return numVerts * sizeof(Vertex); }
	GLsizeiptr indexBufferSize() const { return numIndices * sizeof(unsigned short); }
};