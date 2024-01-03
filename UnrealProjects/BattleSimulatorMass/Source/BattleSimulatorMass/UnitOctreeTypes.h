// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "Math/GenericOctree.h"


typedef TSharedRef<struct FUnitOctreeId, ESPMode::ThreadSafe> FUnitOctreeIdSharedRef;

struct FUnitOctreeId : public TSharedFromThis<FUnitOctreeId, ESPMode::ThreadSafe>
{
	FOctreeElementId2 Id;
};

struct FUnitOctreeElement
{
	FMassEntityHandle EntityHandle;
	FBoxSphereBounds Bounds;
	FUnitOctreeIdSharedRef SharedOctreeId;

	FUnitOctreeElement(const FMassEntityHandle InEntityHandle, const FBoxSphereBounds& InBounds, const FUnitOctreeIdSharedRef& InSharedOctreeId)
		: EntityHandle(InEntityHandle)
		, Bounds(InBounds)
		, SharedOctreeId(InSharedOctreeId)
	{
	}
};

struct FUnitOctreeSemantics
{
	enum { MaxElementsPerLeaf = 64 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	FORCEINLINE static const FBoxSphereBounds& GetBoundingBox(const FUnitOctreeElement& Element)
	{
		return Element.Bounds;
	}

	FORCEINLINE static bool AreElementsEqual(const FUnitOctreeElement& A, const FUnitOctreeElement& B)
	{
		return A.EntityHandle == B.EntityHandle;
	}

	FORCEINLINE static void SetElementId(const FUnitOctreeElement& Element, FOctreeElementId2 Id)
	{
		Element.SharedOctreeId->Id = Id;
	}
};

typedef TOctree2<FUnitOctreeElement, FUnitOctreeSemantics> FUnitOctree;
