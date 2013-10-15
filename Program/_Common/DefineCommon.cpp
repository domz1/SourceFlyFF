
// date : 2009/08/05
// gmpbigsun : for dynamic MPU

#include "StdAfx.h"
#include "DefineCommon.h"

// global for server and client----------------------------------------------------

//MPU
const int		 OLD_MPU = 4 ;						// gmpbigsun : 예전 MPU값
int				 g_MPU = OLD_MPU;					// 09_08_04 "wld"파일에서 불러들인 값이 없다면 예전값을 쓴다.
//-----------------------------------------------------------

