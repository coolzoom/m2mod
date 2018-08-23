#include "M2Types.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <algorithm>

M2Lib::CTriangle::CTriangle()
{
	TriangleIndex = 0;

	Vertices[0] = 0;
	Vertices[1] = 0;
	Vertices[2] = 0;
}

M2Lib::CTriangle::CTriangle(const CTriangle& Other)
{
	*this = Other;
}

M2Lib::CTriangle& M2Lib::CTriangle::operator = (const CTriangle& Other)
{
	TriangleIndex = Other.TriangleIndex;

	Vertices[0] = Other.Vertices[0];
	Vertices[1] = Other.Vertices[1];
	Vertices[2] = Other.Vertices[2];

	return *this;
}

M2Lib::CVertex::CVertex()
{
	for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
	{
		BoneWeights[i] = 0;
		BoneIndices[i] = 0;
	}
}

M2Lib::CVertex::CVertex(const CVertex& Other)
{
	*this = Other;
}

M2Lib::CVertex& M2Lib::CVertex::operator = (const CVertex& Other)
{
	Position = Other.Position;
	Normal = Other.Normal;

	for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
	{
		BoneWeights[i] = Other.BoneWeights[i];
		BoneIndices[i] = Other.BoneIndices[i];
	}

	Texture = Other.Texture;
	Texture2 = Other.Texture2;

	return *this;
}

// compares 2 vertices to see if they have the same position, bones, and texture coordinates. vertices between subsets that pass this test are most likely duplicates.
bool M2Lib::CVertex::CompareSimilar(CVertex& A, CVertex& B, bool CompareTextures, bool CompareBones, Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// compare position
	PositionalTolerance = PositionalTolerance * PositionalTolerance;
	if (PositionalTolerance > 0.0f)
	{
		C3Vector Delta = A.Position - B.Position;
		if (Delta.Length() > sqrtf(PositionalTolerance))
			return false;
	}
	else
	{
		if (!floatEq(A.Position.X, B.Position.X) || !floatEq(A.Position.Y, B.Position.Y) || !floatEq(A.Position.Z, B.Position.Z))
			return false;
	}

	// compare texture coordinates
	if (CompareTextures)
	{
		if (!floatEq(A.Texture.X, B.Texture.X) || !floatEq(A.Texture.Y, B.Texture.Y))
			return false;
	}

	// compare bones
	if (CompareBones)
	{
		// order independent comparison
		bool SameBones[BONES_PER_VERTEX];
		for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
			SameBones[i] = false;

		for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
		{
			bool HasSameBone = false;
			for (UInt32 j = 0; j < BONES_PER_VERTEX; ++j)
			{
				if (A.BoneIndices[i] == B.BoneIndices[j] && SameBones[j] == false)
				{
					SameBones[j] = true;
					break;
				}
			}
		}
		if (!(SameBones[0] && SameBones[1] && SameBones[2] && SameBones[3]))
			return false;
	}

	// compare normals
	// one way to visualize this is that NormalA defines a plane and NormalB defines a point
	// we want to determine what side of the PlaneA that the PointB lies on
	// it's just as simple as getting the dot product of the two vectors and checking the sign of the result
	// arc cosine the dot product of the vectors to get the angle between them
	if (AngularTolerance > 0.0f)
	{
		Float32 Dot = A.Normal.X * B.Normal.X + A.Normal.Y * B.Normal.Y + A.Normal.Z * B.Normal.Z;
		if (acosf(Dot) > AngularTolerance)	// units are radians
			return false;
	}
	else
	{
		if (!floatEq(A.Normal.X, B.Normal.Y) || !floatEq(A.Normal.Y, B.Normal.Y) || !floatEq(A.Normal.Z, B.Normal.Z))
			return false;
	}

	return true;
}

const char* M2Lib::GetErrorText(EError Error)
{
	switch (Error)
	{
	case EError_OK:
		return "ok";

	case EError_FAIL:
		return "error: unspecified error.";

	case EError_PathTooLong:
		return "error: file path is too long.";

	case EError_FailedToLoadM2_NoFileSpecified:
		return "error: failed to load M2, no file was specified.";

	case EError_FailedToLoadM2_CouldNotOpenFile:
		return "error: failed to load M2, could not open file.";

	case EError_FailedToLoadM2_FileCorrupt:
		return "error: failed to load M2, file is corrupt.";

	case EError_FailedToLoadM2_VersionNotSupported:
		return "error: failed to load M2, format version is not supported.";

	case EError_FailedToExportM2I_NoFileSpecified:
		return "error: failed to export M2I, no file was specified.";

	case EError_FailedToExportM2I_CouldNotOpenFile:
		return "error: failed to export M2I, could not open file.";

	case EError_FailedToExportM2I_M2NotLoaded:
		return "error: failed to export M2I, no M2 was loaded.";

	case EError_FailedToImportM2I_NoFileSpecified:
		return "error: failed to import M2I, no file was specified.";

	case EError_FailedToImportM2I_CouldNotOpenFile:
		return "error: failed to import M2I, could not open file.";

	case EError_FailedToImportM2I_FileCorrupt:
		return "error: failed to import M2I, file is corrupt.";

	case EError_FailedToImportM2I_UnsupportedVersion:
		return "error: failed to import M2I, version number unsupported.";

	case EError_FailedToImportM2I_TooManyVertices:
		return "error: failed to import M2I, it has too many vertices (more than 65535).";

	case EError_FailedToImportM2I_SkinHasTooManyIndices:
		return "error: failed to import M2I, generated skin has too many indices (more than 65535).";

	case EError_FailedToSaveM2:
		return "error: failed to save M2, unspecified error.";

	case EError_FailedToSaveM2_NoFileSpecified:
		return "error: failed to save M2, no file was specified.";

	case EError_FailedToLoadSKIN_NoFileSpecified:
		return "error: failed to load SKIN, file not specified.";

	case EError_FailedToLoadSKIN_CouldNotOpenFile:
		return "error: failed to load SKIN, could not open file. make sure you put the M2 and skins in the same folder.";

	case EError_FailedToLoadSKIN_FileMissingOrCorrupt:
		return "error: failed to load SKIN, file is missing or corrupt.";

	case EError_FailedToSaveSKIN:
		return "error: faled to save SKIN, unspecified error.";

	case EError_FailedToSaveSKIN_NoFileSpecified:
		return "error: faled to save SKIN, no file was specified.";
	case EError_FailedToSaveM2BR_Ambiguous:
		return "error: failed to save M2BR file, remap is ambiguous.";

	}

	return "error: unrecognized error";
}

M2Lib::SubmeshExtraData::SubmeshExtraData()
{
	ID = 0;
	FirstLODMeshIndex = -1;
	MaterialOverride = -1;
	TextureStyle = 2;
	OriginalSubmeshIndex = -1;
}

const int M2Lib::BoundaryData::ExtraData::BoundingTriangleVertexMap[BOUNDING_TRIANGLE_COUNT * 3] =
{
	0, 1, 2,	// top
	0, 2, 3,

	6, 3, 2,	// right
	6, 2, 7,

	0, 4, 1,	// left
	0, 5, 4,

	2, 1, 7,	//	back
	1, 4, 7,

	0, 3, 5,	// front
	3, 6, 5,

	4, 5, 7,	// bottom
	5, 6, 7
};

void M2Lib::BoundaryData::Calculate(std::vector<CVertex> const& vertices)
{
	CenterMass = C3Vector();

	bool FirstPass = true;
	for (auto const& Vertex : vertices)
	{
		if (FirstPass)
		{
			BoundingMin = Vertex.Position;
			BoundingMax = Vertex.Position;

			FirstPass = false;
		}
		else
		{
			if (BoundingMin.X > Vertex.Position.X)
				BoundingMin.X = Vertex.Position.X;
			if (BoundingMin.Y > Vertex.Position.Y)
				BoundingMin.Y = Vertex.Position.Y;
			if (BoundingMin.Z > Vertex.Position.Z)
				BoundingMin.Z = Vertex.Position.Z;

			if (BoundingMax.X < Vertex.Position.X)
				BoundingMax.X = Vertex.Position.X;
			if (BoundingMax.Y < Vertex.Position.Y)
				BoundingMax.Y = Vertex.Position.Y;
			if (BoundingMax.Z < Vertex.Position.Z)
				BoundingMax.Z = Vertex.Position.Z;
		}

		CenterMass = CenterMass + Vertex.Position;
	}

	CenterMass = CenterMass / (Float32)vertices.size();

	SortCenter = (BoundingMin + BoundingMax) / 2.0f;

	SortRadius = 0.0f;
	for (auto const& Vertex : vertices)
	{
		C3Vector PositionLocal = Vertex.Position - SortCenter;

		Float32 Distance = PositionLocal.Length();
		if (Distance > SortRadius)
			SortRadius = Distance;
	}
}

M2Lib::BoundaryData::ExtraData M2Lib::BoundaryData::CalculateExtra() const
{
	ExtraData Extra;

	Extra.BoundingVertices[0] = BoundingMax;
	Extra.BoundingVertices[3] = Extra.BoundingVertices[2] = Extra.BoundingVertices[1] = Extra.BoundingVertices[0];
	Extra.BoundingVertices[1].X *= -1.0f;
	Extra.BoundingVertices[2].X *= -1.0f; Extra.BoundingVertices[2].Y *= -1.0f;
	Extra.BoundingVertices[3].Y *= -1.0f;

	Extra.BoundingVertices[7] = BoundingMin;
	Extra.BoundingVertices[4] = Extra.BoundingVertices[5] = Extra.BoundingVertices[6] = Extra.BoundingVertices[7];
	Extra.BoundingVertices[4].Y *= -1.0f;
	Extra.BoundingVertices[6].X *= -1.0f;
	Extra.BoundingVertices[5].X *= -1.0f; Extra.BoundingVertices[5].Y *= -1.0f;

	for (int i = 0; i < BOUNDING_TRIANGLE_COUNT; ++i)
	{
		Extra.BoundingNormals[i] = C3Vector::CalculateNormal(
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3]],
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3 + 1]],
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3 + 2]]
			);
	}

	return Extra;
}

M2Lib::C2Vector& M2Lib::C2Vector::operator = (const C2Vector& Other)
{
	X = Other.X;
	Y = Other.Y;

	return *this;
}

M2Lib::C3Vector& M2Lib::C3Vector::operator = (const C3Vector& Other)
{
	X = Other.X;
	Y = Other.Y;
	Z = Other.Z;

	return *this;
}

M2Lib::C3Vector M2Lib::C3Vector::operator + (const C3Vector& Other) const
{
	C3Vector result;
	result.X = X + Other.X;
	result.Y = Y + Other.Y;
	result.Z = Z + Other.Z;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator - (const C3Vector& Other) const
{
	C3Vector result;
	result.X = X - Other.X;
	result.Y = Y - Other.Y;
	result.Z = Z - Other.Z;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator * (Float32 Value) const
{
	C3Vector result;
	result.X = X * Value;
	result.Y = Y * Value;
	result.Z = Z * Value;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator / (Float32 Value) const
{
	C3Vector result;
	result.X = X / Value;
	result.Y = Y / Value;
	result.Z = Z / Value;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::CrossProduct(C3Vector const& other) const
{
	C3Vector Product;

	Product.X = Y * other.Z - Z * other.Y;
	Product.Y = Z * other.X - X * other.Z;
	Product.Z = X * other.Y - Y * other.X;

	return Product;
}

Float32 M2Lib::C3Vector::Length() const
{
	return sqrtf(X * X + Y * Y + Z * Z);
}

void M2Lib::C3Vector::Normalize()
{
	Float32 length = Length();

	X = std::min(X / length, 1.0f);
	Y = std::min(Y / length, 1.0f);
	Z = std::min(Z / length, 1.0f);
}

M2Lib::C3Vector M2Lib::C3Vector::CalculateNormal(C3Vector const& v1, C3Vector const& v2, C3Vector const& v3)
{
	C3Vector V = v2 - v1;
	C3Vector W = v3 - v1;

	C3Vector N = V.CrossProduct(W);
	N.Normalize();
	return N;
}

bool M2Lib::floatEq(Float32 a, Float32 b)
{
	return std::fabsf(a - b) < 1e-5;
}
