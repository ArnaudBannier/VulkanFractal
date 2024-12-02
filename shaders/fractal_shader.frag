#version 450

// In
layout(location = 0) in vec2 inUV;

// Out
layout(location = 0) out vec4 outColor;

// Uniform
layout(binding = 0) uniform UniformBufferObject {
    dvec2 z0;
    dvec2 z1;
    dvec2 c;
    bool juliaSet;
    int maxIter;

    float colorExponent;
    float colorCycles;
    
    vec3 paletteBase;
    vec3 paletteAmplitude;
    vec3 paletteFrequency;
    vec3 palettePhase;
} ubo;

/// @brief Calcule, pour l'ensemble de Mandelbrot, le rapport du rang de divergence
/// par le nombre maximal d'itérations pour un point du plan complexe.
/// @param c le nombre complexe.
/// @param maxIter le nombre maximal d'itérations.
/// @return Le rapport ou 1.0 si la suite n'a pas divergée après le nombre maximal d'itérations.
float mandelbrotSet(dvec2 c, int maxIter)
{
    dvec2 z = dvec2(0.0, 0.0);

    for (int i = 0; i < maxIter; i++)
    {
        z = dvec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
        if (dot(z, z) > 4.0) return float(i) / float(maxIter);
        
        //if (dot(z, z) > 65536.0)
        //{
        //    float l = log(float(dot(z, z))) / 2.0;
        //    float nu = log(l / log(2.0)) / log(2.0);
        //    return float(i + 1 - nu) / float(ubo.maxIter);
        //}
    }
    return 1.0;
}

/// @brief Calcule, pour l'ensemble de Julia, le rapport du rang de divergence
/// par le nombre maximal d'itérations pour un point du plan complexe.
/// @param c le nombre complexe.
/// @param z0 le nombre complexe.
/// @param maxIter le nombre maximal d'itérations.
/// @return Le rapport ou 1.0 si la suite n'a pas divergée après le nombre maximal d'itérations.
float juliaSet(dvec2 c, dvec2 z0, int maxIter)
{
    dvec2 z = z0;

    for (int i = 0; i < maxIter; i++)
    {
        z = dvec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
        if (dot(z, z) > 4.0) return float(i) / float(maxIter);
        
        //if (dot(z, z) > 65536.0)
        //{
        //    float l = log(float(dot(z, z))) / 2.0;
        //    float nu = log(l / log(2.0)) / log(2.0);
        //    return float(i + 1 - nu) / float(ubo.maxIter);
        //}
    }
    return 1.0;
}

/// @brief Calcule la couleur RGB dans la palette basée sur le cosinus.
/// @param t la position dans la palette (entre 0 et 1)
/// @param base la base de la palette pour chaque canal RGB.
/// @param amp l'amplitude de la palette pour chaque canal RGB.
/// @param freq la fréquence de la palette pour chaque canal RGB.
/// @param phase la phase de la palette pour chaque canal RGB.
/// @return La couleur associée au paramètre t.
vec3 cosPalette(float t, vec3 base, vec3 amp, vec3 freq, vec3 phase)
{
    return base + amp * cos(6.28318 * (freq * t + phase));
}

/// @brief Calcule la fonction de coloration cyclique exponentielle.
/// @param t le paramètre (entre 0 et 1).
/// @param s l'exposant.
/// @param n le nombre de cycles au carré.
/// @return La valeur de la coloration cyclique exponentielle.
float cyclicMap(float t, float s, float n)
{
    return mod(pow(pow(t, s) * n, 1.5), n) / n;
}

void main()
{
    dvec2 zIn = dvec2(
        (1.0 - double(inUV.x)) * ubo.z0.x + double(inUV.x) * ubo.z1.x,
	    (1.0 - double(inUV.y)) * ubo.z0.y + double(inUV.y) * ubo.z1.y
    );

    float t = 0.0;
    if (ubo.juliaSet)
    {
        t = juliaSet(ubo.c, zIn, ubo.maxIter);
    }
    else
    {
        t = mandelbrotSet(zIn, ubo.maxIter);
    }

    if (t >= 1.0)
    {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 paletteColor = cosPalette(
        cyclicMap(t, ubo.colorExponent, ubo.colorCycles),
        ubo.paletteBase,
        ubo.paletteAmplitude,
        ubo.paletteFrequency,
        ubo.palettePhase);
    
    outColor = vec4(paletteColor, 1.0);
}