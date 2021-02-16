#version 450 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D ourTexture;
uniform bool RenderRed;

void main()
{
	FragColor = texture(ourTexture, TexCoords);

	if (RenderRed)
		FragColor = FragColor * vec4(1.0, 0.0, 0.0, 1.0);
}