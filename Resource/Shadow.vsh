vs.1.1

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;


; 역라이트벡터와 버텍스노말벡터와의 내적을 구해서 r0에 넣음.
dp3 r0.x, v1, -c92	; c92=LightDir

;라이트위치에서 버텍스까지의 벡터를 얻음 
sub r1, v0, c91		; c91=LightPos

;If r0.x is less than zero, then this vertex is facing away
;from the light. If that's the case, push it farther.
slt r2.x, r0.x, c95.y	; r0.x가 c95.y(0)과 비교해서 음수인지 체크 음수라면 r2.x는 1 아니면 0
mul r2.x, r2.x, c95.w    ; c95.w=PushDistance
mad r3, r2.x, r1, v0

;Do the final transformation and emit it in oPos. This
;is completely independent of the lighting operations
dp4 oPos.x, r3, c84
dp4 oPos.y, r3, c85
dp4 oPos.z, r3, c86
dp4 oPos.w, r3, c87


