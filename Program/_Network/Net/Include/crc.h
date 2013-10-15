#ifndef CRYPTOPP_CRC32_H
#define CRYPTOPP_CRC32_H

typedef unsigned int word32;
typedef unsigned char byte;
const word32 CRC32_NEGL = 0xffffffffL;

//인텔 계열의 CPU는 리틀인디언 
#define IS_LITTLE_ENDIAN 

#ifdef IS_LITTLE_ENDIAN
#define CRC32_INDEX(c) (c & 0xff)
#define CRC32_SHIFTED(c) (c >> 8)
#else
#define CRC32_INDEX(c) (c >> 24)
#define CRC32_SHIFTED(c) (c << 8)
#endif

//! CRC Checksum Calculation
class CRC32
{
public:
	enum {DIGESTSIZE = 4};
	CRC32();
	void Restart()           	{TruncatedFinal(NULL, 0);}
	void Final(byte *digest)	{TruncatedFinal(digest, DigestSize());}

	void Update(const byte *input, unsigned int length);
	void TruncatedFinal(byte *hash, unsigned int size);
	unsigned int DigestSize() const {return DIGESTSIZE;}

	void UpdateByte(byte b) {m_crc = m_tab[CRC32_INDEX(m_crc) ^ b] ^ CRC32_SHIFTED(m_crc);}
	byte GetCrcByte(unsigned int i) const {return ((byte *)&(m_crc))[i];}

private:
	void Reset() {m_crc = CRC32_NEGL;}
	void ThrowIfInvalidTruncatedSize(unsigned int size) const;
	
	static const word32 m_tab[256];
	word32 m_crc;
};



#endif // CRYPTOPP_CRC32_H
