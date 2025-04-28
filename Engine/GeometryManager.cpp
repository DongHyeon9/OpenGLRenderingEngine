#include "GeometryManager.h"

bool GeometryManager::Init()
{
	return true;
}

MeshData GeometryManager::CreatePlane(float Width, float Height, uint32 RowSlice, uint32 ColumnSlice) const
{
	MeshData result{};

	//Vertex 및 Index 배열 참조
	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	uint32 rowCount = RowSlice + 1;
	uint32 colCount = ColumnSlice + 1;

	//실질적으로 사용할 Width와 Height는 GLOBAL::UNIT을 나눈 값
	float unitWidth = Width * GLOBAL::UNIT;
	float unitHeight = Height * GLOBAL::UNIT;

	//RowSlice와 ColumnSlice에 따른 단위 길이 계산
	float rowInterval = unitWidth / rowCount;	// 행 간의 간격
	float colInterval = unitHeight / colCount;	// 열 간의 간격

	//Vertex 생성
	for (uint32 i = 0; i <= colCount; ++i)
	{
		for (uint32 j = 0; j <= rowCount; ++j)
		{
			Vertex v{};

			//x, z 위치 계산 (y는 고정, z 방향으로 내려가며 격자 생성)
			float x = -unitWidth / 2.0f + j * rowInterval;
			float z = -unitHeight / 2.0f + i * colInterval;
			v.position = Vector4(x, 0.0f, z, 1.0f);

			//노말은 상단을 향하도록 설정
			v.normal = GLOBAL::UP;

			//텍스처 좌표 (u, v) 계산
			v.texcoord = Vector2(static_cast<float>(j) / rowCount, 1.0f - static_cast<float>(i) / colCount);

			v.tangent = GLOBAL::RIGHT;

			//버텍스 배열에 추가
			vertices.emplace_back(v);
		}
	}

	// 인덱스 생성 (삼각형으로 평면을 구성)
	for (uint32 i = 0; i < colCount; ++i)
	{
		for (uint32 j = 0; j < rowCount; ++j)
		{
			// 인덱스 계산
			uint32 index0 = i * (rowCount + 1) + j;
			uint32 index1 = index0 + 1;
			uint32 index2 = index0 + (rowCount + 1);
			uint32 index3 = index2 + 1;

			// 첫 번째 삼각형
			indices.emplace_back(index0);
			indices.emplace_back(index2);
			indices.emplace_back(index1);

			// 두 번째 삼각형
			indices.emplace_back(index1);
			indices.emplace_back(index2);
			indices.emplace_back(index3);
		}
	}
	return result;
}

MeshData GeometryManager::CreateBox(const Vector3& BoxExtent) const
{
	MeshData result{};

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	const float xUnit = BoxExtent.x * GLOBAL::UNIT * 0.5f;
	const float yUnit = BoxExtent.y * GLOBAL::UNIT * 0.5f;
	const float zUnit = BoxExtent.z * GLOBAL::UNIT * 0.5f;

	std::vector<Vector4> positions{};
	std::vector<Vector4> normals{};
	std::vector<Vector2> texcoords{};
	std::vector<Vector4> tangents{};

	//정면
	positions.emplace_back(Vector4(-xUnit, yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, -yUnit, -zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::BACKWARD);
		tangents.emplace_back(GLOBAL::LEFT);
	}

	//오른쪽
	positions.emplace_back(Vector4(xUnit, yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, -zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::RIGHT);
		tangents.emplace_back(GLOBAL::BACKWARD);
	}

	//왼쪽
	positions.emplace_back(Vector4(-xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, -yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, -yUnit, zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::LEFT);
		tangents.emplace_back(GLOBAL::FORWARD);
	}

	//상단
	positions.emplace_back(Vector4(-xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, yUnit, -zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::UP);
		tangents.emplace_back(GLOBAL::LEFT);
	}

	//하단
	positions.emplace_back(Vector4(-xUnit, -yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, -zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, -yUnit, zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::DOWN);
		tangents.emplace_back(GLOBAL::LEFT);
	}

	//후면
	positions.emplace_back(Vector4(xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(-xUnit, -yUnit, zUnit, 1.0f));
	positions.emplace_back(Vector4(xUnit, -yUnit, zUnit, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 0.0f));
	texcoords.emplace_back(Vector2(1.0f, 1.0f));
	texcoords.emplace_back(Vector2(0.0f, 1.0f));
	for (size_t i = 0; i < 4; ++i)
	{
		normals.emplace_back(GLOBAL::FORWARD);
		tangents.emplace_back(GLOBAL::LEFT);
	}


	indices = {
		0,1,3,1,2,3,		//정면
		4,5,7,5,6,7,		//오른쪽
		8,9,11,9,10,11,		//왼쪽
		12,13,15,13,14,15,	//상단
		16,17,19,17,18,19,	//하단
		20,21,23,21,22,23	//후면
	};

	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v{};
		v.position = positions[i];
		v.normal = normals[i];
		v.texcoord = texcoords[i];
		v.tangent = tangents[i];
		vertices.emplace_back(v);
	}

	return result;
}

MeshData GeometryManager::CreateCapsule(float Radius, float Height, uint32 NumSlices, uint32 HemiSphereStacks) const
{
	MeshData result{};

	// 입력값의 단위화
	const float unitRadius = Radius * GLOBAL::UNIT;
	const float unitHeight = Height * GLOBAL::UNIT;
	const float halfHeight = unitHeight * 0.5f;

	//최솟값 설정
	NumSlices += 3;
	HemiSphereStacks += 1;
	HemiSphereStacks = HemiSphereStacks & 1 ? HemiSphereStacks + 1 : HemiSphereStacks;
	uint32 sphereStack = HemiSphereStacks * 2;

	//각 Slice와 Stack에 대한 각도 계산

	const float sliceAngle = -glm::tau<float>() / NumSlices;
	const float stackAngle = -glm::pi<float>() / sphereStack;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//버텍스 생성
	for (uint32 i = 0; i <= sphereStack; ++i) {
		const float phi = stackAngle * i;

		Vector4 stackStartPoint = Vector4(0.0f, -unitRadius, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), phi, Vector3(0, 0, 1));

		for (uint32 j = 0; j <= NumSlices; ++j) {
			const float theta = sliceAngle * j;

			Vertex v{};
			v.position = stackStartPoint * glm::rotate(glm::mat4(1.0f), sliceAngle * static_cast<float>(j), Vector3(0, 1, 0));
			v.normal = v.position;
			v.normal = glm::normalize(v.normal);
			v.texcoord = Vector2(static_cast<float>(j) / NumSlices, 0.0f);

			v.tangent.x = -Radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = Radius * sinf(phi) * cosf(theta);
			v.tangent = glm::normalize(v.tangent);

			vertices.emplace_back(v);
		}
	}

	//텍스처 좌표 조정을 위한 값 계산
	const float totalHeight = Radius * 2 + Height;
	const float radiusRatio = Radius / totalHeight;
	const float texcoordUnit = radiusRatio / (HemiSphereStacks - 1);
	size_t halfIndex = static_cast<size_t>((sphereStack * 0.5f) * (NumSlices + 1));

	//상단반구
	for (size_t i = halfIndex; i < vertices.size(); ++i)
	{
		//캡슐 중앙에 올 버텍스들
		if (i < halfIndex + NumSlices + 1)
		{
			vertices[i].position.y = 0.0f;
			vertices[i].texcoord.y = 0.5f;
		}
		//나머지 버텍스는 위로
		else
		{
			vertices[i].position = vertices[i].position * glm::translate(glm::mat4(1.0f), Vector3(0.0f, halfHeight, 0.0f));
			vertices[i].texcoord.y = radiusRatio - texcoordUnit * ((i - halfIndex - NumSlices - 1) / (NumSlices + 1));
		}
	}
	LOG("상단 반구 이동, 텍스처 좌표 조정");

	//하단반구
	for (size_t i = 0; i < halfIndex; ++i)
	{
		//하단 반구 이동
		vertices[i].position = vertices[i].position * glm::translate(glm::mat4(1.0f), Vector3(0.0f, -halfHeight, 0.0f));
		vertices[i].texcoord.y = 1.0f - texcoordUnit * (i / (NumSlices + 1));
	}
	LOG("하단 반구 이동, 텍스처 좌표 조정");

	//인덱스 연결
	for (uint32 i = 0; i < sphereStack; ++i)
	{
		const uint32 offset = (NumSlices + 1) * i;
		for (uint32 j = 0; j < NumSlices; ++j)
		{

			indices.emplace_back(offset + j);
			indices.emplace_back(offset + j + NumSlices + 1);
			indices.emplace_back(offset + j + 1 + NumSlices + 1);

			indices.emplace_back(offset + j);
			indices.emplace_back(offset + j + 1 + NumSlices + 1);
			indices.emplace_back(offset + j + 1);
		}
	}
	return result;
}

MeshData GeometryManager::CreateCylinder(float BottomRadius, float TopRadius, float Height, uint32 NumSlices) const
{
	MeshData result{};

	NumSlices += 3;

	//길이 단위화, 각도 계산
	const float unitAngle = -glm::tau<float>() / NumSlices;
	const float unitBottomRadius = BottomRadius * GLOBAL::UNIT;
	const float unitTopRadius = TopRadius * GLOBAL::UNIT;
	const float unitHeight = Height * GLOBAL::UNIT;
	const float halfHeight = unitHeight * 0.5f;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//하단 버텍스 생성
	for (uint32 i = 0; i <= NumSlices; i++) {
		Vertex v{};
		v.position = Vector4(unitBottomRadius, -halfHeight, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), unitAngle * i, Vector3(0, 1, 0));
		v.normal = v.position;
		v.normal = glm::normalize(v.normal);
		v.texcoord = Vector2(static_cast<float>(i) / NumSlices, 1.0f);
		v.tangent = GLOBAL::DOWN;

		vertices.emplace_back(v);
	}

	//상단 버텍스 생성
	for (uint32 i = 0; i <= NumSlices; i++) {
		Vertex v{};

		v.position = Vector4(unitTopRadius, halfHeight, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), unitAngle * i, Vector3(0, 1, 0));
		v.normal = v.position;
		v.normal = glm::normalize(v.normal);
		v.texcoord = Vector2(static_cast<float>(i) / NumSlices, 0.0f);
		v.tangent = GLOBAL::UP;

		vertices.emplace_back(v);
	}

	//인덱스 연결
	for (uint32 i = 0; i < NumSlices; i++) {
		indices.emplace_back(i);
		indices.emplace_back(i + NumSlices + 1);
		indices.emplace_back(i + 1 + NumSlices + 1);

		indices.emplace_back(i);
		indices.emplace_back(i + 1 + NumSlices + 1);
		indices.emplace_back(i + 1);
	}

	return result;
}

MeshData GeometryManager::CreateSphere(float Radius, uint32 NumSlices, uint32 NumStacks) const
{
	MeshData result{};

	// 입력값의 단위화
	const float unitRadius = Radius * GLOBAL::UNIT;

	NumSlices += 3;
	NumStacks += 2;

	//각 Stack과 Slice에 대해 각도 계산
	const float sliceAngle = -glm::tau<float>() / NumSlices;
	const float stackAngle = -glm::pi<float>() / NumStacks;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//버텍스 생성
	for (uint32 i = 0; i <= NumStacks; ++i) {
		const float phi = stackAngle * i;

		Vector4 stackStartPoint = Vector4(0.0f, -unitRadius, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), stackAngle * i, Vector3(0, 0, 1));

		for (uint32 j = 0; j <= NumSlices; ++j) {
			Vertex v{};
			const float theta = sliceAngle * j;

			v.position = stackStartPoint * glm::rotate(glm::mat4(1.0f), sliceAngle * static_cast<float>(j), Vector3(0, 1, 0));
			v.normal = v.position;
			v.normal = glm::normalize(v.normal);
			v.texcoord = Vector2(static_cast<float>(j) / NumSlices, 1.0f - static_cast<float>(i) / NumStacks);

			v.tangent.x = -Radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = Radius * sinf(phi) * cosf(theta);
			v.tangent = glm::normalize(v.tangent);

			vertices.emplace_back(v);
		}
	}

	//인덱스 연결
	for (uint32 i = 0; i < NumStacks; ++i)
	{
		const uint32 offset = (NumSlices + 1) * i;
		for (uint32 j = 0; j < NumSlices; ++j)
		{
			indices.emplace_back(offset + j);
			indices.emplace_back(offset + j + NumSlices + 1);
			indices.emplace_back(offset + j + 1 + NumSlices + 1);

			indices.emplace_back(offset + j);
			indices.emplace_back(offset + j + 1 + NumSlices + 1);
			indices.emplace_back(offset + j + 1);
		}
	}
	return result;
}
