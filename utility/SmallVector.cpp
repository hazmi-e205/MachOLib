#include "SmallVector.h"

/// grow_pod - This is an implementation of the grow() method which only works
/// on POD-like datatypes and is out of line to reduce code duplication.
void SmallVectorBase::grow_pod(size_t MinSizeInBytes, size_t TSize) 
{
	size_t CurSizeBytes = size_in_bytes();
	size_t NewCapacityInBytes = 2 * capacity_in_bytes() + TSize; // Always grow.
	if (NewCapacityInBytes < MinSizeInBytes)
		NewCapacityInBytes = MinSizeInBytes;

	void *NewElts;
	if (this->isSmall()) 
	{
		NewElts = malloc(NewCapacityInBytes);

		// Copy the elements over.  No need to run dtors on PODs.
		memcpy(NewElts, this->m_begin, CurSizeBytes);
	} 
	else 
	{
		// If this wasn't grown from the inline copy, grow the allocated space.
		NewElts = realloc(this->m_begin, NewCapacityInBytes);
	}

	this->m_end = (char*)NewElts+CurSizeBytes;
	this->m_begin = NewElts;
	this->m_capacity = (char*)this->m_begin + NewCapacityInBytes;
}

