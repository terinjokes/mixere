// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22feb04	initial version

        smart buffer
 
*/

#ifndef CSMARTBUF_INCLUDED
#define CSMARTBUF_INCLUDED

class CSmartBuf {
public:
// Construction
	CSmartBuf();
	~CSmartBuf();
	CSmartBuf(const CSmartBuf& Buf);
	CSmartBuf& operator=(const CSmartBuf& Buf);

// Attributes
	const	void	*GetData() const;
	DWORD	GetDataLen() const;
	void	SetData(const void *Data, DWORD Len);
	void	SetDataPtr(void *Data, DWORD Len);
	bool	IsEmpty() const;

// Operations
	void	Empty();

private:
// Member data
	void	*m_Data;
	DWORD	m_Len;

// Helpers
	void	Copy(const CSmartBuf& Buf);
};

inline CSmartBuf::CSmartBuf()
{
	m_Data = NULL;
	m_Len = 0;
}

inline CSmartBuf::~CSmartBuf()
{
	delete [] m_Data;
}

inline void CSmartBuf::Copy(const CSmartBuf& Buf)
{
	if (Buf.m_Data != NULL) {
		m_Data = new BYTE[Buf.m_Len];
		memcpy(m_Data, Buf.m_Data, Buf.m_Len);
	} else
		m_Data = NULL;
	m_Len = Buf.m_Len;
}

inline CSmartBuf::CSmartBuf(const CSmartBuf& Buf)
{
	Copy(Buf);
}

inline CSmartBuf& CSmartBuf::operator=(const CSmartBuf& Buf)
{
	delete [] m_Data;
	Copy(Buf);
	return(*this);
}

inline const void *CSmartBuf::GetData() const
{
	return(m_Data);
}

inline DWORD CSmartBuf::GetDataLen() const
{
	return(m_Len);
}

inline void CSmartBuf::SetData(const void *Data, DWORD Len)
{
	delete [] m_Data;
	m_Data = new BYTE[Len];
	m_Len = Len;
	memcpy(m_Data, Data, Len);
}

inline void CSmartBuf::SetDataPtr(void *Data, DWORD Len)
{
	delete [] m_Data;
	m_Data = Data;
	m_Len = Len;
}

inline bool CSmartBuf::IsEmpty() const
{
	return(m_Data == NULL);
}

inline void CSmartBuf::Empty()
{
	SetDataPtr(NULL, 0);
}

#endif
