#pragma once
#include "pch.h"
#include "DVector2.h"
#include "Triangle.h"
#include "Delaunay.h"
#include "Frustum.h"
#include "TerrainShader.h"

using namespace std;
using namespace DirectX;

const int MAX_TRIANGLES = 10000;

class Terrain
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
		float walkable = 0.0f;
	};

	struct VoronoiPoint {
		float x, y, z;
		int index;
		float height;
		int RegionIndex;
	};

	struct VoronoiData {
		VoronoiPoint* VorPoint;
		float dist;
	};

	struct VoronoiRegion {
		vector<int> VRegionIndices;
		float maxDist = 0.0f;
		VoronoiPoint* vPoint;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		VoronoiData *VorData;
		float walkable = 0.0f;
	};

	struct VectorType
	{
		float x, y, z;
		float walkable = 0.0f;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		NodeType* nodes[4];
		VectorType* vertexArray;
		int* mainTextureIndex;
	};

public:
	Terrain();
	~Terrain();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*, int indexCount);
	bool GenerateHeightMap(ID3D11Device*);
	void RandomHeightMap(ID3D11Device*);
	void Faulting(ID3D11Device*);
	void NoiseHeightMap(ID3D11Device*);
	void VoronoiDungeon(ID3D11Device*);
	bool SmoothenHeightMap(ID3D11Device*, float smoothenFactor = 1.f);
	float* GetWavelength();

	int GetIndexCount() { return m_indexCount; }
	int GetVertexCount() { return m_vertexCount; }
	VertexType* GetVertexArray() { return m_vertices; }
	std::vector<VoronoiRegion*> GetRooms() { return m_rooms; }

	bool GetHeightAtPosition(float, float, float&, bool& canWalk);

	float* GetAmplitude();
	void ReleaseVoronoi();
	void Shutdown();

private:
	bool CalculateNormals();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void CalculateTextureCoordinates();

	void RandomHeightMap();
	
	void Faulting();

	void NoiseHeightMap();;

	double simplexNoise(double x, double y);
	float randomFloat(float a, float b);
	int fastfloor(double x);
	double dot(int g[], double x, double y);
	double mix(double a, double b, double t);
	double fade(double t);

	void VoronoiDungeon();

	void AddVoronoiPointAt(int IndexInArray, int RegionIndex);
	void VoronoiRegions(int numOfPoints, int numOfRooms);
	void DelaunayTriangles();
	bool isCircular(vector<Edge*>& edges);
	bool isCircular(int v, bool visited[], vector<int, allocator<int>>** adj, int parent);
	void makeCorridors(const vector<Edge*>& tree);
	float GetWalkableValue(float i, float j);

	void ReinitializeBuffers(ID3D11Device* device);
	int GetDrawCount();
	Vector3 CalculateMeshDimensions(int);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ResetNodeBuffers(NodeType*, ID3D11Device*);
	void ReleaseNode(NodeType*);
	//void RenderNode(NodeType*, Frustum*, ID3D11DeviceContext*, TerrainShader*);
	void FindNode(NodeType*, float, float, float&, bool& canWalk);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount, m_dungeonDepth;
	VertexType* m_vertices;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;
	HeightMapType* m_newHeightMap;

	int grad3[12][3] = { {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1} };
	int p[256] = { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167, 43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
	int perm[512];

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;

	// Voronoi Variables
	vector<VoronoiRegion*>* m_VRegions;
	vector<VoronoiPoint*>* m_VPoints;
	std::vector<VoronoiRegion*>	m_rooms;
	std::vector<std::vector<HeightMapType*>> m_corridors;

	// QuadTree Variables
	int m_triangleCount, m_drawCount;
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

