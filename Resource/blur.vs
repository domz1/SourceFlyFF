vs.1.1
;def c0, -0.0009765625, 0, 0, 0		// offset
;def c0,  0, 0, 1.0, 0		// offset
;def c1,  0, 0, 0, 0
;def c2,  0.0009765625, 0, 0, 0

dcl_position v0
dcl_texcoord v1

mov oPos, v0
;mov oD0, c0

;mov oT0, v1

add oT0, v1, c0
add oT1, v1, c1
add oT2, v1, c2
