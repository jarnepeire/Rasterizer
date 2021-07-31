#pragma once
#include "EMath.h"
#include <string>
#include <vector>
#include "Structs.h"

class ObjParser final
{
public:
	ObjParser(const char* filepath, bool isDX);
	ObjParser(const ObjParser& o) = delete;
	ObjParser(ObjParser&& o) = delete;
	ObjParser& operator=(const ObjParser& o) = delete;
	ObjParser& operator=(ObjParser&& o) = delete;

	/* Copies over the IndexBuffer to the passed parameter */
	void LoadIndexBuffer(std::vector<uint32_t>& indexBuffer) { indexBuffer = m_IndexBuffer; }

	/* Copies over the VertexBuffer to the passed parameter */
	void LoadVertexBuffer(std::vector<Vertex_Input>& vertexBuffer) { vertexBuffer = m_VertexBuffer; }

private:
	/* Starts parsing information from file to put in buffers 
		if IsDX = true, it will invert z-component of Position, Normals and Tangents */
	void InitializeParsing();

	bool m_IsDX;
	std::string m_FilePath;
	std::vector<uint32_t> m_IndexBuffer;
	std::vector<Vertex_Input> m_VertexBuffer;
};

