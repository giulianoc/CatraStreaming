
// #include "mp4av_common.h"
#include "mbs.h"

void CMemoryBitstream::AllocBytes(unsigned long numBytes) 
{
	m_pBuf = (unsigned char*)calloc(numBytes, 1);
	if (!m_pBuf) {
		// throw ENOMEM;
		return;
	}
	m_bitPos = 0;
	m_numBits = numBytes << 3;
}

void CMemoryBitstream::SetBytes(unsigned char* pBytes, unsigned long numBytes) 
{
	m_pBuf = pBytes;
	m_bitPos = 0;
	m_numBits = numBytes << 3;
}

void CMemoryBitstream::PutBytes(unsigned char* pBytes, unsigned long numBytes)
{
	unsigned long numBits = numBytes << 3;

	if (numBits + m_bitPos > m_numBits) {
		// throw EIO;
		return;
	}

	if ((m_bitPos & 7) == 0) {
		memcpy(&m_pBuf[m_bitPos >> 3], pBytes, numBytes);
		m_bitPos += numBits;
	} else {
		for (unsigned long i = 0; i < numBytes; i++) {
			PutBits(pBytes[i], 8);
		}
	}
}

void CMemoryBitstream::PutBits(unsigned long bits, unsigned long numBits)
{
	if (numBits + m_bitPos > m_numBits) {
		// throw EIO;
		return;
	}
	if (numBits > 32) {
		// throw EIO;
		return;
	}

	for (char i = numBits - 1; i >= 0; i--) {
		m_pBuf[m_bitPos >> 3] |= ((bits >> i) & 1) << (7 - (m_bitPos & 7));
		m_bitPos++;
	}
}

unsigned long CMemoryBitstream::GetBits(unsigned long numBits)
{
	if (numBits + m_bitPos > m_numBits) {
		// throw EIO;
		return (unsigned long) -1;
	}
	if (numBits > 32) {
		// throw EIO;
		return (unsigned long) -1;
	}

	unsigned long bits = 0;

	for (unsigned char i = 0; i < numBits; i++) {
		bits <<= 1;
		bits |= (m_pBuf[m_bitPos >> 3] >> (7 - (m_bitPos & 7))) & 1;
		m_bitPos++;
	}

	return bits;
}

