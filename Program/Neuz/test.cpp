
DWORD dwWidth;
DWORD dwHeight;

// Lock the texture to access the pixel data
D3DLOCKED_RECT rect;
HRESULT hResult = pTexture->LockRect(0, &rect, NULL, D3DLOCK_READONLY);
if(FAILED(hResult))
{
   // Error: Can't lock texture for some reason
   return false;
}
// Allocate the bitmask
bool* pCollisionMask = new bool[dwWidth * dwHeight];

// Process the texture, line by line
for(DWORD y=0; y<dwHeight; ++y)
{
   // Get a pointer to the start of this scanline in the texture
   DWORD* pBits = (DWORD*)((BYTE*)rect.pBits + y*rect.Pitch); // See note 1 below

   // Process this line of pixels
   for(DWORD x=0; x<dwWidth; ++x)
   {
      // Get this pixel
      DWORD dwPixel = pBits[x];

      // If the alpha is 0x00 (transparent), set this entry of the collision mask to false, else set it to true
      pCollisionMask[y*dwWidth + x] = (dwPixel&0xff000000) != 0x00;
   }
}
// Done with the texture, so unlock it
pTexture->UnlockRect(0);