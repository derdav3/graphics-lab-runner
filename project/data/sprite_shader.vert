
uniform mediump mat4 ViewMatrix;
uniform mediump mat4 ModelMatrix;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

uniform mediump vec4 borders;
// borders[0] : minX
// borders[1] : maxX
// borders[2] : minY
// borders[3] : maxY

uniform mediump vec4 bordersXMin;
uniform mediump vec4 bordersXMax;
uniform mediump vec4 bordersYMin;
uniform mediump vec4 bordersYMax;


attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec4 TexCoord;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;        // pos in world space
varying mediump vec3 normalVarying;     // normal in world space
varying mediump vec3 tangentVarying;    // tangent in world space

varying vec4 vVertex;

uniform mediump vec4 PlayerPosition;

varying mediump vec4 PlayerPosVarying;

void main()
{
    posVarying = ModelMatrix * Position;
    normalVarying = normalize(NormalMatrix * Normal);
    tangentVarying = normalize(NormalMatrix * Tangent);
    texCoordVarying = TexCoord;
    
//        if (texCoordVarying.x > bordersXMax) {
//            texCoordVarying.x = texCoordVarying.x - bordersXMax;
//        } else if (texCoordVarying.x < bordersXMin) {
//            texCoordVarying.x = texCoordVarying.x + bordersXMin;
//        } else {
//            texCoordVarying.x = texCoordVarying.x/0.5;
//        }
//        if (texCoordVarying.y > bordersYMax) {
//            texCoordVarying.y = bordersYMax;
//        } else if (texCoordVarying.y < bordersYMin) {
//            texCoordVarying.y = bordersYMin;
//        } else {
//            texCoordVarying.y = texCoordVarying.y;
//        }

    
    if (texCoordVarying.x > borders[1]) {
        texCoordVarying.x = borders[1];
    } else if (texCoordVarying.x < borders[0]) {
        texCoordVarying.x = borders[0];
    } else {
        texCoordVarying.x = texCoordVarying.x;
    }
    if (texCoordVarying.y > borders[3]) {
        texCoordVarying.y = borders[3];
    } else if (texCoordVarying.y < borders[2]) {
        texCoordVarying.y = borders[2];
    } else {
        texCoordVarying.y = texCoordVarying.y;
    }
    texCoordVarying.t = texCoordVarying.t;
    //texCoordVarying.t = 0f;
    PlayerPosVarying = PlayerPosition;
    
    // TL:
    // BL: x>0.5,y>0.5
    // BR: x<0.5,y>0.5
    //
    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
    vVertex = gl_Position;
    
    
}
