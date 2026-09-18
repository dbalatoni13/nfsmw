// librealshapez.a(shpcreate.cpp) - copias fuera de linea de TexelTypeHelper.
//
// Las dos funciones viven al final del .text del juego (0x803A4180) y no
// llevan `gcc2_compiled.` delante: son las copias `.gnu.linkonce.t` de dos
// metodos definidos DENTRO de la clase. Aqui se escriben fuera de linea, que
// emite el mismo cuerpo con el mismo simbolo.
//
// No se incluye realshape.hpp a proposito: alli TexelTypeHelper esta definido
// en clase (para que creates.cpp y model.cpp lo inlinen) y volver a definirlo
// aqui chocaria.

namespace RealShape {

enum TexelType {
    TEXEL_TYPE_INVALID = 0,
    TEXEL_TYPE_CLUT_FIRST = 0x30,
    TEXEL_TYPE_CLUT_LAST = 0x33
};

// La tabla la define creates.cpp.
extern const unsigned char gTexelTypeToBpp[256];

class TexelTypeHelper {
  public:
    static int IsClutType(TexelType texelType);
    static int GetDepth(TexelType texelType);
};

int TexelTypeHelper::IsClutType(TexelType texelType) {
    int first = TEXEL_TYPE_CLUT_FIRST;
    int last = TEXEL_TYPE_CLUT_LAST;
    return texelType <= last && texelType >= first;
}

int TexelTypeHelper::GetDepth(TexelType texelType) {
    return gTexelTypeToBpp[(unsigned char)texelType];
}

} // namespace RealShape
