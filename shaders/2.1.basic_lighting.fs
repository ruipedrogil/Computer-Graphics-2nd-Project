#version 330 core
out vec4 FragColor;

in vec3 FragPos;  // Posição do fragmento no espaço mundial
in vec3 Normal;   // Normal interpolada do fragmento

// Posição da luz e do observador (câmera)
uniform vec3 light_position;  // Posição da luz (corrigido para evitar conflito com a palavra-chave "light.position")
uniform vec3 viewPos;         // Posição da câmera

// Estrutura para propriedades do material
struct Material {
    vec3 ambient;  // Cor base do material
    vec3 diffuse;  // Cor da luz difusa refletida
    vec3 specular; // Cor do brilho especular
    float shininess; // Intensidade do brilho especular
};
uniform Material material;

// Estrutura para propriedades da luz
struct Light {
    vec3 ambient;  // Intensidade da luz ambiente
    vec3 diffuse;  // Intensidade da luz difusa
    vec3 specular; // Intensidade da luz especular
};
uniform Light light;

void main() {
    // Luz ambiente (reduzida para evitar saturação)
    vec3 ambient = light.ambient * material.ambient * 0.4; // 40% da luz ambiente

    // Luz difusa
    vec3 norm = normalize(Normal); // Normalização da normal
    vec3 lightDir = normalize(light_position - FragPos); // Direção da luz
    float diff = max(dot(norm, lightDir), 0.0); // Intensidade difusa
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Luz especular
    vec3 viewDir = normalize(viewPos - FragPos); // Direção da câmera
    vec3 reflectDir = reflect(-lightDir, norm);  // Reflexão da luz
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); // Intensidade especular
    vec3 specular = light.specular * (spec * material.specular);

    // Cor base para evitar saturação branca
    vec3 baseColor = vec3(0.7, 0.7, 0.7); // Cinza claro

    // Combinação das componentes
    vec3 result = baseColor * (ambient + diffuse + specular);

    // Cor final do fragmento
    FragColor = vec4(result, 1.0);
}
