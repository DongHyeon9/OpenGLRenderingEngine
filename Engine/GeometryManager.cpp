#include "GeometryManager.h"

bool GeometryManager::Init()
{
	return true;
}

MeshData GeometryManager::CreatePlane(float Width, float Height, uint32 RowSlice, uint32 ColumnSlice) const
{
	MeshData result{};

	//Vertex �� Index �迭 ����
	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	uint32 rowCount = RowSlice + 1;
	uint32 colCount = ColumnSlice + 1;

	//���������� ����� Width�� Height�� GLOBAL::UNIT�� ���� ��
	float unitWidth = Width * GLOBAL::UNIT;
	float unitHeight = Height * GLOBAL::UNIT;

	//RowSlice�� ColumnSlice�� ���� ���� ���� ���
	float rowInterval = unitWidth / rowCount;	// �� ���� ����
	float colInterval = unitHeight / colCount;	// �� ���� ����

	//Vertex ����
	for (uint32 i = 0; i <= colCount; ++i)
	{
		for (uint32 j = 0; j <= rowCount; ++j)
		{
			Vertex v{};

			//x, z ��ġ ��� (y�� ����, z �������� �������� ���� ����)
			float x = -unitWidth / 2.0f + j * rowInterval;
			float z = -unitHeight / 2.0f + i * colInterval;
			v.position = Vector4(x, 0.0f, z, 1.0f);

			//�븻�� ����� ���ϵ��� ����
			v.normal = GLOBAL::UP;

			//�ؽ�ó ��ǥ (u, v) ���
			v.texcoord = Vector2(static_cast<float>(j) / rowCount, 1.0f - static_cast<float>(i) / colCount);

			v.tangent = GLOBAL::RIGHT;

			//���ؽ� �迭�� �߰�
			vertices.emplace_back(v);
		}
	}

	// �ε��� ���� (�ﰢ������ ����� ����)
	for (uint32 i = 0; i < colCount; ++i)
	{
		for (uint32 j = 0; j < rowCount; ++j)
		{
			// �ε��� ���
			uint32 index0 = i * (rowCount + 1) + j;
			uint32 index1 = index0 + 1;
			uint32 index2 = index0 + (rowCount + 1);
			uint32 index3 = index2 + 1;

			// ù ��° �ﰢ��
			indices.emplace_back(index0);
			indices.emplace_back(index2);
			indices.emplace_back(index1);

			// �� ��° �ﰢ��
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

	//����
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

	//������
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

	//����
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

	//���
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

	//�ϴ�
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

	//�ĸ�
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
		0,1,3,1,2,3,		//����
		4,5,7,5,6,7,		//������
		8,9,11,9,10,11,		//����
		12,13,15,13,14,15,	//���
		16,17,19,17,18,19,	//�ϴ�
		20,21,23,21,22,23	//�ĸ�
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

	// �Է°��� ����ȭ
	const float unitRadius = Radius * GLOBAL::UNIT;
	const float unitHeight = Height * GLOBAL::UNIT;
	const float halfHeight = unitHeight * 0.5f;

	//�ּڰ� ����
	NumSlices += 3;
	HemiSphereStacks += 1;
	HemiSphereStacks = HemiSphereStacks & 1 ? HemiSphereStacks + 1 : HemiSphereStacks;
	uint32 sphereStack = HemiSphereStacks * 2;

	//�� Slice�� Stack�� ���� ���� ���

	const float sliceAngle = -glm::tau<float>() / NumSlices;
	const float stackAngle = -glm::pi<float>() / sphereStack;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//���ؽ� ����
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

	//�ؽ�ó ��ǥ ������ ���� �� ���
	const float totalHeight = Radius * 2 + Height;
	const float radiusRatio = Radius / totalHeight;
	const float texcoordUnit = radiusRatio / (HemiSphereStacks - 1);
	size_t halfIndex = static_cast<size_t>((sphereStack * 0.5f) * (NumSlices + 1));

	//��ܹݱ�
	for (size_t i = halfIndex; i < vertices.size(); ++i)
	{
		//ĸ�� �߾ӿ� �� ���ؽ���
		if (i < halfIndex + NumSlices + 1)
		{
			vertices[i].position.y = 0.0f;
			vertices[i].texcoord.y = 0.5f;
		}
		//������ ���ؽ��� ����
		else
		{
			vertices[i].position = vertices[i].position * glm::translate(glm::mat4(1.0f), Vector3(0.0f, halfHeight, 0.0f));
			vertices[i].texcoord.y = radiusRatio - texcoordUnit * ((i - halfIndex - NumSlices - 1) / (NumSlices + 1));
		}
	}
	LOG("��� �ݱ� �̵�, �ؽ�ó ��ǥ ����");

	//�ϴܹݱ�
	for (size_t i = 0; i < halfIndex; ++i)
	{
		//�ϴ� �ݱ� �̵�
		vertices[i].position = vertices[i].position * glm::translate(glm::mat4(1.0f), Vector3(0.0f, -halfHeight, 0.0f));
		vertices[i].texcoord.y = 1.0f - texcoordUnit * (i / (NumSlices + 1));
	}
	LOG("�ϴ� �ݱ� �̵�, �ؽ�ó ��ǥ ����");

	//�ε��� ����
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

	//���� ����ȭ, ���� ���
	const float unitAngle = -glm::tau<float>() / NumSlices;
	const float unitBottomRadius = BottomRadius * GLOBAL::UNIT;
	const float unitTopRadius = TopRadius * GLOBAL::UNIT;
	const float unitHeight = Height * GLOBAL::UNIT;
	const float halfHeight = unitHeight * 0.5f;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//�ϴ� ���ؽ� ����
	for (uint32 i = 0; i <= NumSlices; i++) {
		Vertex v{};
		v.position = Vector4(unitBottomRadius, -halfHeight, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), unitAngle * i, Vector3(0, 1, 0));
		v.normal = v.position;
		v.normal = glm::normalize(v.normal);
		v.texcoord = Vector2(static_cast<float>(i) / NumSlices, 1.0f);
		v.tangent = GLOBAL::DOWN;

		vertices.emplace_back(v);
	}

	//��� ���ؽ� ����
	for (uint32 i = 0; i <= NumSlices; i++) {
		Vertex v{};

		v.position = Vector4(unitTopRadius, halfHeight, 0.0f, 1.0f) * glm::rotate(glm::mat4(1.0f), unitAngle * i, Vector3(0, 1, 0));
		v.normal = v.position;
		v.normal = glm::normalize(v.normal);
		v.texcoord = Vector2(static_cast<float>(i) / NumSlices, 0.0f);
		v.tangent = GLOBAL::UP;

		vertices.emplace_back(v);
	}

	//�ε��� ����
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

	// �Է°��� ����ȭ
	const float unitRadius = Radius * GLOBAL::UNIT;

	NumSlices += 3;
	NumStacks += 2;

	//�� Stack�� Slice�� ���� ���� ���
	const float sliceAngle = -glm::tau<float>() / NumSlices;
	const float stackAngle = -glm::pi<float>() / NumStacks;

	std::vector<Vertex>& vertices = result.vertices;
	std::vector<uint32>& indices = result.indices;

	//���ؽ� ����
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

	//�ε��� ����
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
