varying vec2 v_coord;
varying vec4 DestinationColor;

#Vertex_Begin

#Attribute
{
	POSITION:position;
	COLOR:color;
	TEXTURECOORD:inputTextureCoordinate;
	NORMAL:attr_normal;
}

#include "Standard.shader"

void main(void)
{
    v_coord = inputTextureCoordinate;
	vec4 interpolatedPosition = vec4(position ,1.0);	
	//position.x = position.x + 1.0;
	mat4 rotateMatrix = getRotateMatrix();
	vec4 _normal = rotateMatrix * vec4(attr_normal.xyz, 1.0);
	v_coord = inputTextureCoordinate;
	vec4 _pos = _e3d_matModel * interpolatedPosition;
	DestinationColor = getLightColor(_pos.xyz, _normal.xyz, 100.0) * color;
	initFogNeedVar(position);
    gl_Position = _e3d_getMVPMatrix() * vec4(position ,1.0);
}

#Vertex_End

#Framgent_Begin

#ifdef __GLES__
precision highp float;
#endif
#include "Fragment.shader"
uniform sampler2D myTexture0;

void main(void) 
{ 
	gl_FragColor = mixFogColor(texture2D(myTexture0, v_coord) * DestinationColor, vec4(1.0,1.0,1.0,1.0));	
}

#Framgent_End