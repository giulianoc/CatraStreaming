
#ifndef mbs_h
	#define mbs_h 

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	class CMemoryBitstream {
		public:
			CMemoryBitstream() {
				m_pBuf = NULL;
				m_bitPos = 0;
				m_numBits = 0;
			}

			void AllocBytes(unsigned long numBytes);

			void SetBytes(unsigned char* pBytes, unsigned long numBytes);

			void PutBytes(unsigned char* pBytes, unsigned long numBytes);

			void PutBits(unsigned long bits, unsigned long numBits);

			unsigned long GetBits(unsigned long numBits);

			void SkipBytes(unsigned long numBytes) {
				SkipBits(numBytes << 3);
			}

			void SkipBits(unsigned long numBits) {
				SetBitPosition(GetBitPosition() + numBits);
			}

			unsigned long GetBitPosition() {
				return m_bitPos;
			}

			void SetBitPosition(unsigned long bitPos) {
				if (bitPos > m_numBits) {
					throw;
				}
				m_bitPos = bitPos;
			}

			unsigned char* GetBuffer() {
				return m_pBuf;
			}

			unsigned long GetNumberOfBytes() {
				return (GetNumberOfBits() + 7) / 8;
			}

			unsigned long GetNumberOfBits() {
				return m_numBits;
			}

		protected:
			unsigned char*	m_pBuf;
			unsigned long	m_bitPos;
			unsigned long	m_numBits;
	};

#endif

