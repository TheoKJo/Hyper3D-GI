/*!
 * \file		BinaryDataStream.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/12/12
 * created:		2011/12/12 18:23
 * copyright	Hybrid (creavol@gmail.com)
 */

#pragma once

class BinaryDataStream
{
public:
	BinaryDataStream()
		: m_DataSize(0), m_BinaryData(NULL)
	{
	}
	BinaryDataStream( unsigned int DataSize )
		: m_DataSize(0), m_BinaryData(NULL)
	{
		// TODO: exception?
		AllocateData(DataSize);
	}
	~BinaryDataStream()
	{
		DestroyData();
	}

	bool AllocateData( unsigned int DataSize )
	{
		DestroyData();
		if( DataSize == 0 )
			return true;
		m_DataSize = DataSize;
		m_BinaryData = new char[DataSize];
		return m_BinaryData != NULL;
	}
	void DestroyData()
	{
		m_DataSize = 0;
		SafeDeleteArray( &m_BinaryData );
	}

	unsigned int GetBinaryDataSize() const
	{
		return m_DataSize;
	}
	char* GetBinaryData()
	{
		return m_BinaryData;
	}
	const char* GetBinaryData() const 
	{
		return m_BinaryData;
	}

	BinaryDataStream operator=( const BinaryDataStream &bds )
	{
		AllocateData( bds.GetBinaryDataSize() );
		for( unsigned int i = 0; i < m_DataSize; i++ )
		{
			m_BinaryData[i] = bds.GetBinaryData()[i];
		}

		return *this;
	}

	unsigned int AddBinaryDataStream( const BinaryDataStream &bds )
	{
		unsigned int newDataSize = m_DataSize + bds.GetBinaryDataSize();
		char *newBinaryData = new char[newDataSize];

		for( unsigned int i = 0; i < m_DataSize; i++ )
			newBinaryData[i] = m_BinaryData[i];
		for( unsigned int i = 0; i < bds.GetBinaryDataSize(); i++ )
			newBinaryData[m_DataSize+i] = bds.GetBinaryData()[i];

		DestroyData();
		m_DataSize = newDataSize;
		m_BinaryData = newBinaryData;
		return newDataSize;
	}
	
private:
	unsigned int m_DataSize;
	char *m_BinaryData;
};

class BinaryDataStreamRequired
{
public:
	virtual bool FromBinaryDataStream( const BinaryDataStream &in ) = 0;
	virtual unsigned int ToBinaryDataStream( BinaryDataStream *out ) = 0;
};
