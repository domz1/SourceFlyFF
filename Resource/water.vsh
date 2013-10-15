vs.1.1
; Constants:
;
;  c0-c3  - View+Projection matrix
;
;  c4.x   - time
;  c4.y   - 0
;  c4.z   - 0.5
;  c4.w   - 1.0
;
;  c7.x   - pi
;  c7.y   - 1/2pi
;  c7.z   - 2pi
;  c7.w   - 0.05
;
;  c10    - first 4 taylor coefficients for sin(x)

; c5.x  wave scale
; c5.y  wave height
; c5.z  wave direction x
; c5.w  wave direction y


; Decompress position

mul r0.x, v0.x,c5.x
mov r0.y, c4.w       ; 1
mul r0.z, v0.y,c5.x
mov r0.w, c4.w       ; 1

mov r3.z,c4.x
mov r3.w,c4.x
mul r3.z,r3.z,c5.z
mul r3.w,r3.w,c5.w


;;; vert

add r4.x, r0.x,r3.z

; Clamp theta to -pi..pi
add r4.x, r4.x, c7.x
mul r4.x, r4.x, c7.y
frc r4.xy, r4.x
mul r4.x, r4.x, c7.z
add r4.x, r4.x,-c7.x

; Compute first 4 values in sin and cos series
mul r5.y, r4.x, r4.x ; d^2  r5.y=r4.x^2
mul r4.y, r4.x, r5.y ; d^3  r4.y=r4.x^3
mul r4.z, r4.y, r5.y ; d^5  r4.z=r4.x^5
mul r4.w, r4.z, r5.y ; d^7  r4.w=r4.x^7

mul r4, r4, c10      ; sin
dp4 r4.x, r4, c4.w

mov r0.y,r4.x


;;; hori 

add r4.x, r0.z,r3.w

; Clamp theta to -pi..pi
add r4.x, r4.x, c7.x
mul r4.x, r4.x, c7.y
frc r4.xy, r4.x
mul r4.x, r4.x, c7.z
add r4.x, r4.x,-c7.x

; Compute first 4 values in sin and cos series
mul r5.y, r4.x, r4.x ; d^2  r5.y=r4.x^2
mul r4.y, r4.x, r5.y ; d^3  r4.y=r4.x^3
mul r4.z, r4.y, r5.y ; d^5  r4.z=r4.x^5
mul r4.w, r4.z, r5.y ; d^7  r4.w=r4.x^7

mul r4, r4, c10      ; sin
dp4 r4.x, r4, c4.w

add r0.y,r0.y,r4.x



; Scale height
mul r0.y, r0.y, c5.y
; Set color
;add r1.w,r0.y,c4.w
;mul oD0, r1.w,c4.z ; * 0.5
mov oD0,c4.w
mov oD0.w,c4.z

add r0.y, r0.y, c7.z

mul r1.x,v0.x,c4.z
mul r1.y,v0.y,c4.z
sub r1.x,r1.x,r3.z
sub r1.y,r1.y,r3.w
mul oT0.x,r1.x,c7.w
mul oT0.y,r1.y,c7.w
; Transform position

mov r0.x, v0.x
mov r0.z, v0.y

dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3

dp4 r0.x, r0, c14
mad oFog.x, r0.x, c6.x, c6.y
