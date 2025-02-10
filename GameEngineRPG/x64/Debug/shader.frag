#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 cameraPos; // **NEW UNIFORM: Camera Position**

uniform sampler2D characterTexture; // **REPLACED texture1/texture2 with characterTexture**

void main()
{
    // **Directional Lighting Calculations**
    vec3 ambient = 0.05 * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDirNormalized = normalize(lightDir);

    // Diffuse
    float diff = max(dot(norm, -lightDirNormalized), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular: Correct view direction calculation
    vec3 viewDir = normalize(cameraPos - FragPos); // **CORRECTED viewDir - using cameraPos**
    vec3 reflectDir = reflect(-lightDirNormalized, norm); // Correctly reflect around *negative* light direction for directional light
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.3 * spec * lightColor;

    // Texture sampling (simplified - just characterTexture)
    vec4 textureColor = texture(characterTexture, TexCoords); // **Directly sample characterTexture**

    vec3 finalColor = ambient + diffuse + specular;
    FragColor = textureColor * vec4(finalColor, 1.0);
}