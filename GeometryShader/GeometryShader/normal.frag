#version 430 core


uniform vec3 lightPos;



in vec3 gNormal;
in vec3 gPosition;



out vec4 fColor;

float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

float red( float gray ) {
    return base( gray - 0.5 );
}
float green( float gray ) {
    return base( gray );
}
float blue( float gray ) {
    return base( gray + 0.5 );
}

vec3 colorGrading(float t)
{
	float a = pow(t,1.0/2.2);
	a = t;
	float gray = 2*a-1; // gray in -1,1
	return vec3(red(gray),green(gray),blue(gray));
}



void main()
{

	float normalMagitude = length(gNormal);
	fColor.rgb = colorGrading(normalMagitude);


}