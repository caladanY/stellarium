#ifndef _STELVERTEXBUFFERBACKEND_HPP_
#define _STELVERTEXBUFFERBACKEND_HPP_

#include <QVector>

#include "StelVertexAttribute.hpp"

//! Base class of all vertex buffer backends.
//!
//! This is where the actual vertex buffer logic is implemented.
//!
//! The backend doesn't directly know the vertex type (since templating doesn't mix
//! with virtual inheritance), so it works with raw (ubyte) pointers.
//!
//! Vertex format is described by the attributes data member.
//! Each item in attributes specifies a vertex attribute (such as a position or normal),
//! and these correspond to the data members of the Vertex struct which is
//! a template parameter of the StelVertexBuffer class.
//!
//! StelVertexBufferBackend is always owned by a StelVertexBuffer and can't exist separately.
//! Only the Renderer bacend might have direct access to StelVertexBufferBackend's implementation.
//!
//! When the user asks Renderer to create a vertex buffer, the renderer backend first creates
//! its own implementation of the vertex buffer backend, and then wraps it up in a
//! StelVertexBuffer.
//!
//! @sa StelVertexBuffer, StelRenderer
class StelVertexBufferBackend
{
public:
	//! Construct a StelVertexBufferBackend, specifying attributes of the vertex type.
	StelVertexBufferBackend(const QVector<StelVertexAttribute>& attributes)
		:attributes(attributes)
	{
	}

	//! Required to ensure that derived classes get properly deallocated.
	virtual ~StelVertexBufferBackend(){};
	
	//! Add a new vertex to the buffer.
	//!
	//! StelVertexBuffer guarantees that when this is called,
	//! buffer is not locked and index is in range.
	//!
	//! @param vertexInPtr Pointer to the beginning of the vertex.
	//!                    Data members of the vertex must match vertex attributes of the buffer.
	virtual void addVertex(const quint8* const vertexInPtr) = 0;
	
	//! Read a vertex from the buffer.
	//!
	//! StelVertexBuffer guarantees that when this is called,
	//! buffer is not locked and index is in range.
	//!
	//! @param index        Index of the vertex to read.
	//! @param vertexOutPtr Pointer to the beginning of the output vertex.
	//!                     Data members of the vertex must match vertex attributes of the buffer.
	virtual void getVertex(const uint index, quint8* const vertexOutPtr) const = 0;
	
	//! Rewrite a vertex in the buffer.
	//!
	//! StelVertexBuffer guarantees that when this is called,
	//! buffer is not locked and index is in range.
	//!
	//! @param index       Index of the vertex to set.
	//! @param vertexInPtr Pointer to the beginning of the vertex we're rewriting with.
	//!                    Data members of the vertex must match vertex attributes of the buffer.
	virtual void setVertex(const uint index, const quint8* const vertexInPtr) = 0;
	
	//! Lock the buffer. Must be called before drawing.
	virtual void lock() = 0;
	
	//! Unlock the buffer. Must be called to modify the buffer after drawing.
	virtual void unlock() = 0;
	
	//! Clear the buffer, removing all vertices.
	//!
	//! The backend implementation might reuse previously allocated storage after 
	//! clearing, so calling clear() might be more efficient than destroying 
	//! a buffer and then constructing a new one.
	virtual void clear() = 0;

	//! Assert that the user-specified (in StelVertexBuffer) vertex type is valid.
	void validateVertexType(const uint vertexSize)
	{
		// We have no way of looking at each data member of the vertex type, 
		// but we can at least enforce that their total length matches.
		uint bytes = 0;
		
		for(int attrib = 0; attrib < attributes.size(); ++attrib)
		{
			bytes += attributeSize(attributes[attrib].type);
		}
		
		Q_ASSERT_X(bytes == vertexSize, Q_FUNC_INFO,
		           "Size of the vertex type in bytes doesn't match the sum of sizes of "
		           "all vertex attributes as reported by \"attributes\" data member.");
	}
	
protected:
	//! Specifies vertex attributes in the vertex type.
	const QVector<StelVertexAttribute>& attributes;
};


#endif // _STELVERTEXBUFFERBACKEND_HPP_
