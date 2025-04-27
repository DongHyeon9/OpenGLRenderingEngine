#pragma once
#include "CoreMinimal.h"

class GeometryManager
{
	DECLARE_SINGLETON(GeometryManager);
public:

protected:

private:

public:
	bool Init();
	MeshData CreatePlane(float Width, float Height, uint32 RowSlice, uint32 ColumnSlice) const;
	MeshData CreateBox(const Vector3& BoxExtent) const;
	MeshData CreateCapsule(float Radius, float Height, uint32 NumSlices, uint32 HemiSphereStacks) const;
	MeshData CreateCylinder(float BottomRadius, float TopRadius, float Height, uint32 NumSlices) const;
	MeshData CreateSphere(float Radius, uint32 NumSlices, uint32 NumStacks) const;

protected:

private:

};

