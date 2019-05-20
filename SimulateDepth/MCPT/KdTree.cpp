#include "Common.h"

using namespace std;


KdTree::KdTree(vector<KdTree*> & basicObjectList, int splitAxis)
{
	BuildTree(basicObjectList, splitAxis);
}

void KdTree::BuildTree(vector<KdTree*> & basicObjecList, int splitAxis)
{

	SplitAxis = splitAxis;
	unsigned int length = basicObjecList.size();

	if (length == 1)
	{
		MinPoint = basicObjecList[0];
		MaxPoint = NULL;
		box = MinPoint->GetAixAlignBoundingBox();
	}
	else if (length == 2) 
	{
		MinPoint = basicObjecList[0];
		MaxPoint = basicObjecList[1];
		box = AixAlignBoundingBox::MergeBox(MinPoint->GetAixAlignBoundingBox(), MaxPoint->GetAixAlignBoundingBox());
	}
	else 
	{
		vector<KdTree*> lowVec;
		vector<KdTree*> highVec;
		partition(SplitAxis, basicObjecList, &lowVec, &highVec);

		if (lowVec.size() > 0) 
		{
			MinPoint = new KdTree(lowVec, (SplitAxis + 1) % 3);
		}
		else
		{
			MinPoint = NULL;
		}

		if (highVec.size() > 0)
		{
			MaxPoint = new KdTree(highVec, (SplitAxis + 1) % 3);
		}
		else
		{
			MaxPoint = NULL;
		}

		if (MinPoint == NULL)
		{
			box = MaxPoint->GetAixAlignBoundingBox();
		}
		else if (MaxPoint == NULL)
		{
			box = MinPoint->GetAixAlignBoundingBox();
		}
		else
		{
			box = AixAlignBoundingBox::MergeBox(MinPoint->GetAixAlignBoundingBox(), MaxPoint->GetAixAlignBoundingBox());
		}
	}
}


void KdTree::partition(int axis, const vector<KdTree*> & all, vector<KdTree*> * lowVec, vector<KdTree*> * highVec)
{

	float minVal = FLT_MAX;
	float maxVal = -FLT_MAX;

	for (unsigned int i = 0; i < all.size(); i++)
	{
		AixAlignBoundingBox ModelBoundingBox = all[i]->GetAixAlignBoundingBox();
		minVal = min(minVal, ModelBoundingBox.GetMinPointCoord(axis));
		maxVal = max(maxVal, ModelBoundingBox.GetMaxPointCoord(axis));
	}

	float pivot = (maxVal + minVal) / 2;

	for (unsigned int i = 0; i < all.size(); i++)
	{
		AixAlignBoundingBox ModelBoundingBox = all[i]->GetAixAlignBoundingBox();
		float centerCoord = ( ModelBoundingBox.GetMaxPointCoord(axis) + ModelBoundingBox.GetMinPointCoord(axis) ) / 2;
		if (centerCoord < pivot)
		{
			lowVec->push_back(all[i]);
		}
		else
		{
			highVec->push_back(all[i]);
		}
	}

	// Everything ended up in highVec
	if ( highVec->size() == all.size() )
	{
		KdTree * obj;
		int index;
		float minCoord = FLT_MAX;
		for (int i = 0; i < highVec->size(); i++)
		{
			AixAlignBoundingBox ModelBoundingBox = (*highVec)[i]->GetAixAlignBoundingBox();
			float centerCoord = ( ModelBoundingBox.GetMaxPointCoord(axis) + ModelBoundingBox.GetMinPointCoord(axis) ) / 2;
			if (centerCoord < minCoord)
			{
				minCoord = centerCoord;
				index = i;
				obj = (*highVec)[i];
			}
		}
		highVec->erase(highVec->begin() + index);
		lowVec->push_back(obj);
	}
	// Everything ended up in lowVec
	else if ( lowVec->size() == all.size() )
	{
		KdTree * obj;
		int index;
		float maxCoord = -FLT_MAX;
		for (int i = 0; i < lowVec->size(); i++)
		{
			AixAlignBoundingBox ModelBoundingBox = (*lowVec)[i]->GetAixAlignBoundingBox();
			float centerCoord = (ModelBoundingBox.GetMaxPointCoord(axis) + ModelBoundingBox.GetMinPointCoord(axis)) / 2;
			if (centerCoord > maxCoord)
			{
				maxCoord = centerCoord;
				index = i;
				obj = (*lowVec)[i];
			}
		}
		lowVec->erase(lowVec->begin() + index);
		highVec->push_back(obj);
	}
}

AixAlignBoundingBox KdTree::GetAixAlignBoundingBox()
{
	return box;
}

bool KdTree::GetIsIntersect(Ray & ray, Intersection & intersection)
{
	if (!box.IsIntersect(ray))
	{
		return false;
	}

	bool isHit = false;

	if (ray.RayDirection.num[SplitAxis] >= 0)
	{

		if (MinPoint != NULL)
		{
			isHit |= MinPoint->GetIsIntersect(ray, intersection);
		}
		if (MaxPoint != NULL)
		{
			isHit |= MaxPoint->GetIsIntersect(ray, intersection);
		}
	}
	else
	{
		if (MaxPoint != NULL)
		{
			isHit |= MaxPoint->GetIsIntersect(ray, intersection);
		}
		if (MinPoint != NULL)
		{
			isHit |= MinPoint->GetIsIntersect(ray, intersection);
		}
	}
	 
	return isHit;
}