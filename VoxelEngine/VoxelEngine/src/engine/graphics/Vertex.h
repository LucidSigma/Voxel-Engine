#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

#include <array>

enum VertexAttributes
	: unsigned int
{
	Position,
	TextureCoordinates
};

struct Vertex2D
{
	glm::vec2 position;
	glm::vec2 textureCoordinates;
};

struct Vertex3D
{
	glm::vec3 position;
	glm::vec3 textureCoordinates;
};

namespace vertices
{
	constexpr std::array<Vertex2D, 4u> QuadVertices{
		Vertex2D{ { 0.0f, 1.0f }, { 0.0f, 1.0f } },
		Vertex2D{ { 1.0f, 0.0f }, { 1.0f, 0.0f } },
		Vertex2D{ { 0.0f, 0.0f }, { 0.0f, 0.0f } },
		Vertex2D{ { 1.0f, 1.0f }, { 1.0f, 1.0f } }
	};

	constexpr std::array<unsigned short, 6u> QuadIndices{
		0u, 1u, 2u,
		0u, 3u, 1u
	};

	constexpr std::array<float, 24u> CubeVertices{      
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f
	};

	constexpr std::array<unsigned short, 36u> CubeIndices{
		0u, 1u, 2u,
		2u, 3u, 0u,

		1u, 5u, 6u,
		6u, 2u, 1u,

		7u, 6u, 5u,
		5u, 4u, 7u,

		4u, 0u, 3u,
		3u, 7u, 4u,

		4u, 5u, 1u,
		1u, 0u, 4u,

		3u, 2u, 6u,
		6u, 7u, 3u
	};

	constexpr std::array<unsigned short, 36u> InvertedCubeIndices{
		0u, 2u, 1u,
		2u, 0u, 3u,

		1u, 6u, 5u,
		6u, 1u, 2u,

		7u, 5u, 6u,
		5u, 7u, 4u,

		4u, 3u, 0u,
		3u, 4u, 7u,

		4u, 1u, 5u,
		1u, 4u, 0u,

		3u, 6u, 2u,
		6u, 3u, 7u
	};
}

#endif